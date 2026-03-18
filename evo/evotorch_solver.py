"""EvoTorch-based optimizer for Puzzle & Dragons board solving.

Uses evolutionary computation to find optimal move sequences, bypassing the
beam search bottleneck and learning how to play the game by optimising
directly for max combo or a configurable reward function.

Two optimisation strategies are provided:

1. PazusobaProblem  – directly evolves a fixed-length move sequence
   (start position + a sequence of directions).  Works out-of-the-box
   without any neural network.

2. NeuroEvoPazusobaProblem – uses EvoTorch's NEProblem to evolve the
   weights of a small MLP policy that maps the board state at each step
   to a move direction, enabling the solver to generalise across boards.

Pluggable reward functions
--------------------------
Both problem classes accept an optional ``reward_fn`` that lets you optimise
for *any* goal instead of the default max-combo rate.  The signature is::

    def my_reward(
        combos: int,        # total cascaded combos achieved
        max_combo: int,     # theoretical maximum for this board
        board: List[int],   # board state AFTER moves, BEFORE cascade erasure
        row: int,
        col: int,
    ) -> float:             # higher is better

Built-in helpers: ``combo_reward``, ``orb_remaining_reward``.

Generalising to any board
--------------------------
Train a policy on a diverse set of boards with ``train_general_policy()``,
then apply the resulting network to any new board with ``run_policy()``::

    from evotorch_solver import train_general_policy, run_policy

    network = train_general_policy(board_size=30, num_boards=50, num_generations=200)
    result  = run_policy(network, "LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL")
    print(result)

Portability: using results in C / C++
--------------------------------------
Three export helpers let you consume EvoTorch results from C or C++ code
without any Python runtime.

**Direct-sequence result → JSON** (readable with any C JSON library)::

    from evotorch_solver import solve, export_solution_json

    result = solve("LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL", max_steps=50)
    export_solution_json(result, "solution.json")

**Trained network → TorchScript** (load with LibTorch in C++)::

    from evotorch_solver import train_general_policy, export_torchscript

    network = train_general_policy(board_size=30)
    export_torchscript(network, "policy.pt", board_size=30)

    // C++ (LibTorch)
    auto model = torch::jit::load("policy.pt");
    auto dir   = model.forward({obs}).toTensor().argmax(1).item<int64_t>();

**Trained network → C header** (zero runtime dependencies, C99 / C++)::

    from evotorch_solver import train_general_policy, export_weights_header

    network = train_general_policy(board_size=30)
    export_weights_header(network, "pazusoba_policy.h", board_size=30)

    // C / C++
    #include "pazusoba_policy.h"
    int dir = pazusoba_policy_forward(obs);  // 0=up,1=down,2=left,3=right

Usage (direct sequence optimisation)
-------------------------------------
    from evotorch_solver import PazusobaProblem, solve
    state = solve("LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL", max_steps=50)
    print(state)

Usage (neuroevolution)
----------------------
    from evotorch_solver import NeuroEvoPazusobaProblem
    from evotorch.algorithms import PGPE
    from evotorch.logging import StdOutLogger

    problem = NeuroEvoPazusobaProblem("LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL")
    searcher = PGPE(problem, popsize=100, radius_init=2.25,
                    center_learning_rate=0.2, stdev_learning_rate=0.1)
    logger = StdOutLogger(searcher, interval=10)
    searcher.run(200)
"""

from __future__ import annotations

import json
import random
import textwrap
from typing import Callable, List, Optional, Tuple

import torch
import torch.nn as nn
from evotorch import Problem
from evotorch.algorithms import SNES
from evotorch.logging import StdOutLogger
from evotorch.neuroevolution import NEProblem

# ---------------------------------------------------------------------------
# Orb constants (matching C++ ORB_WEB_NAME)
# ---------------------------------------------------------------------------

ORB_WEB_NAME = " RBGLDHJEPT"  # index → char
ORB_COUNT = 11

# The six standard non-special orbs used for random board generation.
STANDARD_ORBS = "RBGLDH"

# Direction indices: 0=up, 1=down, 2=left, 3=right (same order as C++)
DIRECTION_COUNT = 4

# ---------------------------------------------------------------------------
# Reward-function type and built-in reward helpers
# ---------------------------------------------------------------------------

# Signature: (combos, max_combo, board_after_moves, row, col) -> float
RewardFn = Callable[[int, int, List[int], int, int], float]


def combo_reward(
    combos: int,
    max_combo: int,
    board: List[int],
    row: int,
    col: int,
) -> float:
    """Default reward: normalised combo count in [0, 1]."""
    if max_combo == 0:
        return 0.0
    return combos / max_combo


def orb_remaining_reward(
    combos: int,
    max_combo: int,
    board: List[int],
    row: int,
    col: int,
) -> float:
    """Reward that penalises leftover orbs.

    Encourages clearing as many orbs from the board as possible.
    Combines combo rate with a bonus for fewer remaining orbs.
    """
    board_size = row * col
    remaining = sum(1 for o in board if o > 0)
    cleared_ratio = 1.0 - remaining / board_size if board_size > 0 else 0.0
    combo_ratio = (combos / max_combo) if max_combo > 0 else 0.0
    return 0.5 * combo_ratio + 0.5 * cleared_ratio


# ---------------------------------------------------------------------------
# Pure-Python board simulation
# ---------------------------------------------------------------------------


def parse_board(board_str: str) -> Tuple[List[int], int, int]:
    """Parse a board string into a flat list of orb integers plus (row, col)."""
    size = len(board_str)
    if size == 20:
        row, col = 4, 5
    elif size == 30:
        row, col = 5, 6
    elif size == 42:
        row, col = 6, 7
    else:
        raise ValueError(f"Unsupported board size {size}; expected 20, 30 or 42")

    board = []
    for ch in board_str:
        idx = ORB_WEB_NAME.find(ch)
        if idx == -1:
            raise ValueError(f"Unknown orb character '{ch}'")
        board.append(idx)
    return board, row, col


def random_board(board_size: int = 30) -> str:
    """Generate a random board string of the given size.

    *board_size* must be 20 (4×5), 30 (5×6) or 42 (6×7).
    Uses the six standard orb colours defined in :data:`STANDARD_ORBS`.
    """
    if board_size not in (20, 30, 42):
        raise ValueError(f"Unsupported board size {board_size}; expected 20, 30 or 42")
    return "".join(random.choice(STANDARD_ORBS) for _ in range(board_size))


def calc_max_combo(board: List[int], row: int, col: int, min_erase: int = 3) -> int:
    """Estimate the theoretical maximum combos for a board (matches C++ logic)."""
    counter = [0] * ORB_COUNT
    for orb in board:
        counter[orb] += 1

    size = row * col
    threshold = size // 2
    max_combo = 0
    for count in counter:
        combo = count // min_erase
        if count > threshold:
            extra = (count - threshold) * 2 // min_erase
            combo -= extra
        max_combo += combo
    return max(max_combo, 1)


def erase_combo(board: List[int], row: int, col: int, min_erase: int = 3) -> int:
    """Erase all matchable combos from *board* (in-place).

    A combo is a connected group of same-colour orbs that contains at least
    one run of *min_erase* or more in a straight line (row or column).

    Algorithm (matches C++ erase_combo):
      1. Mark every cell that belongs to any horizontal or vertical run of
         length >= min_erase.
      2. Flood-fill the marked cells (same colour, 4-directional adjacency)
         to find connected groups — each group counts as ONE combo.
      3. Zero out (erase) every orb in each group.

    Returns the number of combos erased.
    """
    size = row * col
    in_combo = [False] * size

    # --- mark horizontal runs ---
    for r in range(row):
        c = 0
        while c < col:
            orb = board[r * col + c]
            if orb == 0:
                c += 1
                continue
            run_end = c + 1
            while run_end < col and board[r * col + run_end] == orb:
                run_end += 1
            if run_end - c >= min_erase:
                for k in range(c, run_end):
                    in_combo[r * col + k] = True
            c = run_end

    # --- mark vertical runs ---
    for c in range(col):
        r = 0
        while r < row:
            orb = board[r * col + c]
            if orb == 0:
                r += 1
                continue
            run_end = r + 1
            while run_end < row and board[run_end * col + c] == orb:
                run_end += 1
            if run_end - r >= min_erase:
                for k in range(r, run_end):
                    in_combo[k * col + c] = True
            r = run_end

    # --- flood-fill connected groups (same colour, 4-directional) ---
    visited = [False] * size
    combo_count = 0

    # Scan from bottom-right to top-left (mirrors C++ scan order)
    for start in range(size - 1, -1, -1):
        if not in_combo[start] or visited[start]:
            continue

        orb_type = board[start]
        group: List[int] = []
        queue = [start]
        visited[start] = True

        while queue:
            pos = queue.pop(0)
            group.append(pos)
            r, c = divmod(pos, col)

            for dr, dc in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                nr, nc = r + dr, c + dc
                if 0 <= nr < row and 0 <= nc < col:
                    nidx = nr * col + nc
                    if in_combo[nidx] and not visited[nidx] and board[nidx] == orb_type:
                        visited[nidx] = True
                        queue.append(nidx)

        combo_count += 1
        for pos in group:
            board[pos] = 0

    return combo_count


def move_orbs_down(board: List[int], row: int, col: int) -> None:
    """Apply gravity: move orbs downward to fill empty (0) cells (in-place)."""
    for c in range(col):
        empty_row = -1
        for r in range(row - 1, -1, -1):
            idx = r * col + c
            if board[idx] == 0:
                if empty_row == -1:
                    empty_row = r
            elif empty_row != -1:
                board[empty_row * col + c] = board[idx]
                board[idx] = 0
                empty_row -= 1


def count_combos(board: List[int], row: int, col: int, min_erase: int = 3) -> int:
    """Count total combos (including cascades after gravity) for a board state.

    Does not modify *board* (works on an internal copy).
    """
    working_board = board[:]
    total = 0
    while True:
        new_combos = erase_combo(working_board, row, col, min_erase)
        if new_combos == 0:
            break
        total += new_combos
        move_orbs_down(working_board, row, col)
    return total


def _apply_moves(
    initial_board: List[int],
    row: int,
    col: int,
    start_pos: int,
    directions: List[int],
) -> Tuple[List[int], int]:
    """Apply a move sequence to a copy of the board.

    Returns ``(board_after_moves, final_cursor_position)``.
    Invalid moves (boundary violations, back-tracking) are silently skipped,
    matching the C++ ``expand()`` logic.
    """
    board = initial_board[:]
    board_size = row * col
    dir_offsets = [-col, col, -1, 1]  # up, down, left, right

    curr = start_pos
    prev = start_pos

    for d in directions:
        d = int(d) % DIRECTION_COUNT
        offset = dir_offsets[d]
        nxt = curr + offset

        # Mirror C++ validity checks in expand()
        if nxt == prev:
            continue
        if nxt < 0 or nxt >= board_size:
            continue
        if d == 3 and nxt % col == 0:  # moved right but wrapped to next row
            continue
        if d == 2 and curr % col == 0:  # moved left but curr is on left edge
            continue

        board[curr], board[nxt] = board[nxt], board[curr]
        prev = curr
        curr = nxt

    return board, curr


def simulate_moves(
    initial_board: List[int],
    row: int,
    col: int,
    start_pos: int,
    directions: List[int],
    min_erase: int = 3,
) -> int:
    """Simulate a move sequence on the board and return the combo count.

    *directions* is a list of direction indices (0=up, 1=down, 2=left, 3=right).
    Invalid moves (boundary violations, moving back to previous position) are
    silently skipped, matching the C++ expand() logic.
    """
    board, _ = _apply_moves(initial_board, row, col, start_pos, directions)
    return count_combos(board, row, col, min_erase)


def _valid_directions(curr: int, prev: int, row: int, col: int) -> List[int]:
    """Return valid move directions from (curr, prev), mirroring C++ expand()."""
    board_size = row * col
    dir_offsets = [-col, col, -1, 1]
    valid: List[int] = []
    for d, offset in enumerate(dir_offsets):
        nxt = curr + offset
        if nxt == prev:
            continue
        if nxt < 0 or nxt >= board_size:
            continue
        if d == 3 and nxt % col == 0:
            continue
        if d == 2 and curr % col == 0:
            continue
        valid.append(d)
    return valid


def _potential_score(board: List[int], row: int, col: int) -> float:
    """Heuristic potential used by lookahead planning.

    Rewards near-match structures (pairs/runs) to approximate how promising
    the board is for future combo opportunities.
    """
    s = 0.0

    # Horizontal runs
    for r in range(row):
        c = 0
        while c < col:
            orb = board[r * col + c]
            if orb == 0:
                c += 1
                continue
            end = c + 1
            while end < col and board[r * col + end] == orb:
                end += 1
            run = end - c
            if run == 2:
                s += 0.35
            elif run >= 3:
                s += 0.8
            c = end

    # Vertical runs
    for c in range(col):
        r = 0
        while r < row:
            orb = board[r * col + c]
            if orb == 0:
                r += 1
                continue
            end = r + 1
            while end < row and board[end * col + c] == orb:
                end += 1
            run = end - r
            if run == 2:
                s += 0.35
            elif run >= 3:
                s += 0.8
            r = end

    return s


def solve_with_lookahead(
    board_str: str,
    max_steps: int = 50,
    min_erase: int = 3,
    beam_width: int = 256,
) -> "SolveResult":
    """Solve by explicit discrete lookahead (beam search).

    This is closer to human planning behavior: keep several promising partial
    routes, look multiple steps ahead, and prune weaker futures.
    """
    initial_board, row, col = parse_board(board_str)
    board_size = row * col
    max_combo = calc_max_combo(initial_board, row, col, min_erase)
    dir_names = ["up", "down", "left", "right"]

    states = []
    for start in range(board_size):
        combo = count_combos(initial_board, row, col, min_erase)
        heur = combo * 100.0 + _potential_score(initial_board, row, col)
        states.append(
            {
                "board": initial_board[:],
                "curr": start,
                "prev": start,
                "start": start,
                "dirs": [],
                "combo": combo,
                "heur": heur,
            }
        )

    states.sort(key=lambda s: (s["combo"], s["heur"]), reverse=True)
    states = states[:beam_width]

    for _ in range(max_steps):
        expanded = []
        for st in states:
            valid = _valid_directions(st["curr"], st["prev"], row, col)
            if not valid:
                expanded.append(st)
                continue

            for d in valid:
                offset = [-col, col, -1, 1][d]
                nxt = st["curr"] + offset
                new_board = st["board"][:]
                new_board[st["curr"]], new_board[nxt] = (
                    new_board[nxt],
                    new_board[st["curr"]],
                )
                combo = count_combos(new_board, row, col, min_erase)
                heur = combo * 100.0 + _potential_score(new_board, row, col)
                expanded.append(
                    {
                        "board": new_board,
                        "curr": nxt,
                        "prev": st["curr"],
                        "start": st["start"],
                        "dirs": st["dirs"] + [d],
                        "combo": combo,
                        "heur": heur,
                    }
                )

        dedup = {}
        for st in expanded:
            key = (tuple(st["board"]), st["curr"], st["prev"])
            prev = dedup.get(key)
            if prev is None or (st["combo"], st["heur"]) > (
                prev["combo"],
                prev["heur"],
            ):
                dedup[key] = st

        states = list(dedup.values())
        states.sort(key=lambda s: (s["combo"], s["heur"]), reverse=True)
        states = states[:beam_width]

        if states and states[0]["combo"] >= max_combo:
            break

    best = max(states, key=lambda s: (s["combo"], s["heur"]))
    return SolveResult(
        combo=int(best["combo"]),
        max_combo=max_combo,
        start_pos=int(best["start"]),
        row=row,
        col=col,
        directions=[dir_names[d] for d in best["dirs"]],
        goal=int(best["combo"]) >= max_combo,
    )


# ---------------------------------------------------------------------------
# EvoTorch problem: direct move-sequence optimisation
# ---------------------------------------------------------------------------


class PazusobaProblem(Problem):
    """EvoTorch Problem that evolves a fixed-length move sequence for a given board.

    A solution is encoded as a float32 tensor of length ``1 + max_steps``:
      - ``x[0]``        : starting board position (clamped to [0, board_size-1])
      - ``x[1..N]``     : move directions (rounded to integers in [0, 3])

    Fitness is determined by *reward_fn* (default: normalised combo count).

    Parameters
    ----------
    board_str:
        The board layout string.
    max_steps:
        Maximum number of moves to evolve.
    min_erase:
        Minimum orbs required to form a combo.
    reward_fn:
        Optional callable ``(combos, max_combo, board_after_moves, row, col)
        -> float``.  Defaults to :func:`combo_reward`.

    """

    def __init__(
        self,
        board_str: str,
        max_steps: int = 50,
        min_erase: int = 3,
        reward_fn: Optional[RewardFn] = None,
    ) -> None:
        self.board_str = board_str
        self.max_steps = max_steps
        self.min_erase = min_erase
        self.reward_fn: RewardFn = reward_fn if reward_fn is not None else combo_reward

        initial_board, row, col = parse_board(board_str)
        self.initial_board = initial_board
        self.row = row
        self.col = col
        self.board_size = row * col
        self.max_combo = calc_max_combo(initial_board, row, col, min_erase)

        # Lower / upper bounds for each element of the solution vector
        lower = torch.zeros(1 + max_steps, dtype=torch.float32)
        upper = torch.zeros(1 + max_steps, dtype=torch.float32)
        upper[0] = float(self.board_size - 1)
        upper[1:] = float(DIRECTION_COUNT - 1)

        super().__init__(
            objective_sense="max",
            solution_length=1 + max_steps,
            initial_bounds=(lower, upper),
            dtype=torch.float32,
            num_actors=1,
        )

    # ------------------------------------------------------------------
    def _parse_solution(self, x: torch.Tensor) -> Tuple[int, List[int]]:
        """Parse a solution tensor into (start_pos, list_of_int_directions)."""
        rounded = x.round().long()
        start_pos = int(rounded[0].item())
        start_pos = max(0, min(start_pos, self.board_size - 1))
        directions = [
            max(0, min(int(rounded[i + 1].item()), DIRECTION_COUNT - 1))
            for i in range(self.max_steps)
        ]
        return start_pos, directions

    def _fitness_for_values(self, x: torch.Tensor) -> float:
        """Return fitness for a single solution tensor *x*."""
        start_pos, directions = self._parse_solution(x)
        board_after, _ = _apply_moves(
            self.initial_board, self.row, self.col, start_pos, directions
        )
        combos = count_combos(board_after, self.row, self.col, self.min_erase)
        return self.reward_fn(combos, self.max_combo, board_after, self.row, self.col)

    def _evaluate(self, solution) -> None:
        fitness = self._fitness_for_values(solution.values)
        solution.set_evals(torch.tensor([fitness], dtype=torch.float32))

    # ------------------------------------------------------------------
    def decode(self, solution_values: torch.Tensor) -> dict:
        """Decode a solution tensor into a human-readable dict."""
        start_pos, int_dirs = self._parse_solution(solution_values)
        dir_names = ["up", "down", "left", "right"]
        board_after, _ = _apply_moves(
            self.initial_board, self.row, self.col, start_pos, int_dirs
        )
        combos = count_combos(board_after, self.row, self.col, self.min_erase)
        return {
            "start_pos": start_pos,
            "start_row": start_pos // self.col,
            "start_col": start_pos % self.col,
            "directions": [dir_names[d] for d in int_dirs],
            "combo": combos,
            "max_combo": self.max_combo,
            "goal": combos >= self.max_combo,
        }


# ---------------------------------------------------------------------------
# EvoTorch NEProblem: neuroevolution policy optimisation
# ---------------------------------------------------------------------------

# A small MLP that maps a board state to a move direction.
# Input  : board_size orb one-hot features (board_size * ORB_COUNT floats)
#          + current-position one-hot (board_size floats)
# Output : logit for each of the 4 directions


def _build_policy(
    board_size: int, hidden: int = 64, lookahead_steps: int = 10
) -> nn.Module:
    """Build a 2-hidden-layer MLP policy.

    Parameters
    ----------
    board_size:
        Number of cells on the board (20, 30 or 42).
    hidden:
        Number of units in each hidden layer.
    lookahead_steps:
        Number of moves the network plans ahead simultaneously.
        With ``lookahead_steps=1`` the policy is greedy (picks one move).
        With ``lookahead_steps=10`` (default) the policy outputs 10 planned
        moves at once: at each re-observation cycle it plans the next 10 steps
        before executing them and re-querying the network.

    The output dimension is ``lookahead_steps * DIRECTION_COUNT``.  At
    inference time the logits are reshaped to ``(lookahead_steps, 4)`` and
    an ``argmax`` is taken along the last axis to produce the K planned moves.
    """
    in_dim = board_size * ORB_COUNT + board_size
    out_dim = lookahead_steps * DIRECTION_COUNT
    return nn.Sequential(
        nn.Linear(in_dim, hidden),
        nn.Tanh(),
        nn.Linear(hidden, hidden),
        nn.Tanh(),
        nn.Linear(hidden, out_dim),
    )


class NeuroEvoPazusobaProblem(NEProblem):
    """EvoTorch NEProblem that evolves the weights of a small MLP policy.

    The policy observes the current board state + cursor position and outputs
    logits for the next ``lookahead_steps`` moves simultaneously, enabling the
    network to **plan ahead** rather than choose only one move at a time.

    At each planning cycle the network:

    1. Observes the current board and cursor position.
    2. Outputs ``lookahead_steps * 4`` logits.
    3. Reshapes them to ``(lookahead_steps, 4)`` and takes argmax per row to
       obtain K planned moves.
    4. Executes all K moves (skipping invalid ones), then re-observes.

    This mirrors the way a skilled human solver looks 10–15 moves ahead before
    committing to a direction.  Setting ``lookahead_steps=1`` recovers the
    previous greedy behaviour.

    Once trained, the network can be extracted and applied to any new board
    using :func:`run_policy` without retraining.

    Parameters
    ----------
    board_str:
        The board to optimise on (or a list of boards for multi-board training).
        Use :func:`train_general_policy` to train on many random boards.
    max_steps:
        Maximum number of moves per episode.
    min_erase:
        Minimum orbs required to form a combo.
    hidden:
        Hidden layer size of the policy network.
    lookahead_steps:
        Number of moves planned ahead per network query.  Higher values give
        the policy more context to plan complex sequences.  Defaults to 10.
    num_starts:
        Number of evenly spaced starting positions to sample per evaluation.
        Smaller values speed up fitness evaluation at the cost of coverage.
    reward_fn:
        Optional callable ``(combos, max_combo, board_after_moves, row, col)
        -> float``.  Defaults to :func:`combo_reward`.

    """

    def __init__(
        self,
        board_str: str | List[str],
        max_steps: int = 50,
        min_erase: int = 3,
        hidden: int = 64,
        lookahead_steps: int = 10,
        num_starts: int = 6,
        reward_fn: Optional[RewardFn] = None,
    ) -> None:
        self.min_erase = min_erase
        self.max_steps = max_steps
        self.lookahead_steps = max(1, lookahead_steps)  # clamp; 0 or negative → 1
        self.num_starts = num_starts
        self.reward_fn: RewardFn = reward_fn if reward_fn is not None else combo_reward

        # Support single board or a list of boards for curriculum / multi-board
        if isinstance(board_str, str):
            board_str = [board_str]
        self.board_strs = board_str

        # All boards must have the same size
        self._boards: List[Tuple[List[int], int, int, int]] = []
        for bs in self.board_strs:
            board, row, col = parse_board(bs)
            mc = calc_max_combo(board, row, col, min_erase)
            self._boards.append((board, row, col, mc))

        board_size = self._boards[0][1] * self._boards[0][2]
        self.board_size = board_size

        super().__init__(
            objective_sense="max",
            network=_build_policy(board_size, hidden, self.lookahead_steps),
            num_actors=1,
        )

    # ------------------------------------------------------------------
    def _observe(self, board: List[int], board_size: int, curr: int) -> torch.Tensor:
        """Encode board state + cursor position as a flat float tensor."""
        board_feat = torch.zeros(board_size * ORB_COUNT)
        for i, orb in enumerate(board):
            board_feat[i * ORB_COUNT + orb] = 1.0
        pos_feat = torch.zeros(board_size)
        pos_feat[curr] = 1.0
        return torch.cat([board_feat, pos_feat])

    def _run_episode(
        self,
        network: nn.Module,
        initial_board: List[int],
        row: int,
        col: int,
        max_combo: int,
        start: int,
    ) -> float:
        """Run one episode from *start* and return the reward value.

        The policy is queried at each re-observation point and produces
        ``lookahead_steps`` planned moves at once.  All K moves are executed
        (invalid ones silently skipped) before the network is queried again.
        This allows the network to plan multi-step sequences rather than
        committing one step at a time.
        """
        board_size = row * col
        dir_offsets = [-col, col, -1, 1]
        board = initial_board[:]
        curr = start
        prev = start
        steps_taken = 0

        while steps_taken < self.max_steps:
            obs = self._observe(board, board_size, curr)
            with torch.no_grad():
                logits = network(obs.unsqueeze(0)).squeeze(0)

            # Reshape to (lookahead_steps, 4) and take argmax per planning step.
            planned = (
                logits.reshape(self.lookahead_steps, DIRECTION_COUNT)
                .argmax(dim=1)
                .tolist()
            )

            for d in planned:
                if steps_taken >= self.max_steps:
                    break
                offset = dir_offsets[d]
                nxt = curr + offset

                invalid = (
                    nxt == prev
                    or nxt < 0
                    or nxt >= board_size
                    or (d == 3 and nxt % col == 0)
                    or (d == 2 and curr % col == 0)
                )
                if invalid:
                    steps_taken += 1
                    continue

                board[curr], board[nxt] = board[nxt], board[curr]
                prev = curr
                curr = nxt
                steps_taken += 1

        combos = count_combos(board, row, col, self.min_erase)
        return self.reward_fn(combos, max_combo, board, row, col)

    def _evaluate_network(self, network: nn.Module) -> float:
        """Evaluate network on all boards and return mean reward.

        To keep evaluation tractable, only ``num_starts`` evenly spaced
        starting positions are sampled rather than exhaustively trying all.
        """
        total = 0.0
        for initial_board, row, col, max_combo in self._boards:
            board_size = row * col
            # Sample evenly spaced starts instead of all board_size positions
            step = max(1, board_size // self.num_starts)
            best_reward = 0.0
            for start in range(0, board_size, step):
                reward = self._run_episode(
                    network, initial_board, row, col, max_combo, start
                )
                if reward > best_reward:
                    best_reward = reward
            total += best_reward
        return total / len(self._boards)


# ---------------------------------------------------------------------------
# Generalisation utilities: run_policy and train_general_policy
# ---------------------------------------------------------------------------


def run_policy(
    network: nn.Module,
    board_str: str,
    max_steps: int = 50,
    min_erase: int = 3,
    num_starts: int = 6,
    reward_fn: Optional[RewardFn] = None,
) -> "SolveResult":
    """Apply a trained policy network to any board and return the best result.

    This is the *inference* counterpart to training with
    :class:`NeuroEvoPazusobaProblem`.  A network trained on a diverse set of
    boards (e.g. via :func:`train_general_policy`) can be used here without
    retraining, making it a **common solution** applicable to any board.

    The function automatically detects whether the network was trained with
    ``lookahead_steps > 1`` (output dim > 4) and uses the same K-step
    planning loop as during training: at each re-observation point the network
    plans K moves ahead and executes all of them before re-querying.

    Parameters
    ----------
    network:
        A trained ``nn.Module`` (e.g. from
        ``searcher.status["center"].make_net(params)``).
    board_str:
        Any valid board string (size 20, 30 or 42).
    max_steps:
        Maximum number of moves per episode.
    min_erase:
        Minimum orbs required to form a combo.
    num_starts:
        Number of evenly spaced starting positions to try.
    reward_fn:
        Optional reward function.  Defaults to :func:`combo_reward`.

    Returns
    -------
    SolveResult
        The best result found across all sampled starting positions.

    """
    _reward_fn = reward_fn if reward_fn is not None else combo_reward
    initial_board, row, col = parse_board(board_str)
    board_size = row * col
    max_combo = calc_max_combo(initial_board, row, col, min_erase)
    dir_offsets = [-col, col, -1, 1]
    dir_names = ["up", "down", "left", "right"]

    # Infer lookahead_steps from the last linear layer's output features.
    last_linear = [m for m in network.modules() if isinstance(m, nn.Linear)][-1]
    out_dim = last_linear.out_features
    lookahead_steps = max(1, out_dim // DIRECTION_COUNT)

    def _observe(board: List[int], curr: int) -> torch.Tensor:
        board_feat = torch.zeros(board_size * ORB_COUNT)
        for i, orb in enumerate(board):
            board_feat[i * ORB_COUNT + orb] = 1.0
        pos_feat = torch.zeros(board_size)
        pos_feat[curr] = 1.0
        return torch.cat([board_feat, pos_feat])

    best_reward = -1.0
    best_combos = 0
    best_start = 0
    best_dirs: List[int] = []

    step = max(1, board_size // num_starts)
    for start in range(0, board_size, step):
        board = initial_board[:]
        curr = start
        prev = start
        episode_dirs: List[int] = []
        steps_taken = 0

        while steps_taken < max_steps:
            obs = _observe(board, curr)
            with torch.no_grad():
                logits = network(obs.unsqueeze(0)).squeeze(0)

            # Reshape to (lookahead_steps, 4) and argmax per planning step.
            planned = (
                logits.reshape(lookahead_steps, DIRECTION_COUNT)
                .argmax(dim=1)
                .tolist()
            )

            for d in planned:
                if steps_taken >= max_steps:
                    break
                offset = dir_offsets[d]
                nxt = curr + offset

                invalid = (
                    nxt == prev
                    or nxt < 0
                    or nxt >= board_size
                    or (d == 3 and nxt % col == 0)
                    or (d == 2 and curr % col == 0)
                )
                if invalid:
                    episode_dirs.append(d)
                    steps_taken += 1
                    continue

                board[curr], board[nxt] = board[nxt], board[curr]
                episode_dirs.append(d)
                prev = curr
                curr = nxt
                steps_taken += 1

        combos = count_combos(board, row, col, min_erase)
        reward = _reward_fn(combos, max_combo, board, row, col)
        if reward > best_reward:
            best_reward = reward
            best_combos = combos
            best_start = start
            best_dirs = episode_dirs

    return SolveResult(
        combo=best_combos,
        max_combo=max_combo,
        start_pos=best_start,
        row=row,
        col=col,
        directions=[dir_names[d] for d in best_dirs],
        goal=best_combos >= max_combo,
    )


def train_general_policy(
    board_size: int = 30,
    num_boards: int = 50,
    max_steps: int = 50,
    min_erase: int = 3,
    hidden: int = 64,
    lookahead_steps: int = 10,
    num_starts: int = 6,
    popsize: int = 100,
    num_generations: int = 200,
    reward_fn: Optional[RewardFn] = None,
    verbose: bool = True,
) -> nn.Module:
    """Train a generalised policy across many randomly generated boards.

    The resulting network can solve *any* board of the same size using
    :func:`run_policy` without retraining.

    The policy is trained to plan ``lookahead_steps`` moves ahead at each
    re-observation point (default 10), mirroring how a skilled human solver
    looks multiple moves into the future before committing.

    Parameters
    ----------
    board_size:
        Board size to train on (20, 30 or 42).
    num_boards:
        Number of random boards to generate for training.
    max_steps:
        Maximum moves per episode during training.
    min_erase:
        Minimum orbs required to form a combo.
    hidden:
        Hidden units in the MLP policy.
    lookahead_steps:
        Number of moves the network plans ahead simultaneously per query.
        Higher values encourage longer-horizon planning.  Defaults to 10.
    num_starts:
        Starting positions sampled per board per evaluation.
    popsize:
        SNES population size.
    num_generations:
        Training generations.
    reward_fn:
        Optional reward function.  Defaults to :func:`combo_reward`.
    verbose:
        Print progress to stdout.

    Returns
    -------
    nn.Module
        The trained policy network.

    """
    boards = [random_board(board_size) for _ in range(num_boards)]
    problem = NeuroEvoPazusobaProblem(
        boards,
        max_steps=max_steps,
        min_erase=min_erase,
        hidden=hidden,
        lookahead_steps=lookahead_steps,
        num_starts=num_starts,
        reward_fn=reward_fn,
    )
    searcher = SNES(problem, popsize=popsize, stdev_init=1.0)
    if verbose:
        _ = StdOutLogger(searcher, interval=50)

    searcher.run(num_generations)

    best_params = searcher.status["best"].values
    return problem.make_net(best_params)


# ---------------------------------------------------------------------------
# Convenience solve() function
# ---------------------------------------------------------------------------


class SolveResult:
    """Lightweight result object returned by :func:`solve` and :func:`run_policy`."""

    def __init__(
        self,
        combo: int,
        max_combo: int,
        start_pos: int,
        row: int,
        col: int,
        directions: List[str],
        goal: bool,
    ) -> None:
        self.combo = combo
        self.max_combo = max_combo
        self.start_pos = start_pos
        self.start_row = start_pos // col
        self.start_col = start_pos % col
        self.directions = directions
        self.goal = goal

    def to_dict(self) -> dict:
        """Return a JSON-serialisable dict representation of this result."""
        return {
            "start_pos": self.start_pos,
            "start_row": self.start_row,
            "start_col": self.start_col,
            "directions": self.directions,
            "combo": self.combo,
            "max_combo": self.max_combo,
            "goal": self.goal,
        }

    def __str__(self) -> str:
        return (
            f"Combo: {self.combo}/{self.max_combo}\n"
            f"Start: ({self.start_row}, {self.start_col})\n"
            f"Steps: {len(self.directions)}\n"
            f"Goal: {self.goal}"
        )

    def __repr__(self) -> str:
        return self.__str__()


def solve(
    board_str: str,
    max_steps: int = 50,
    min_erase: int = 3,
    popsize: int = 200,
    num_generations: int = 500,
    num_restarts: int = 4,
    stdev_init: float = 2.25,
    refine_iters: int = 2000,
    lookahead_beam_width: int = 192,
    reward_fn: Optional[RewardFn] = None,
    verbose: bool = True,
) -> SolveResult:
    """Use EvoTorch (SNES) to find an optimal move sequence for *board_str*.

    Parameters
    ----------
    board_str:
        The board layout string (e.g. ``"LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL"``).
    max_steps:
        Maximum number of moves to optimise.
    min_erase:
        Minimum orbs to form a combo.
    popsize:
        Population size for SNES.
    num_generations:
        Total number of generations to run across all restarts.
    num_restarts:
        Number of randomized SNES restarts.
        More restarts help avoid local optima on discrete move sequences.
    stdev_init:
        Initial search distribution standard deviation for SNES.
        A higher value improves exploration early in search.
    refine_iters:
        Number of local discrete refinement mutations to apply after SNES.
        This directly optimizes integer moves and is effective on plateaued runs.
    lookahead_beam_width:
        Width of the discrete lookahead beam planner. If > 0, a planner result
        is computed and compared against the Evo solution.
    reward_fn:
        Optional reward function ``(combos, max_combo, board, row, col) -> float``.
        Defaults to :func:`combo_reward`.
    verbose:
        Print progress to stdout.

    Returns
    -------
    SolveResult
        The best solution found.

    """
    if num_restarts < 1:
        num_restarts = 1

    # Curriculum over path length makes optimization less brittle:
    # first learn shorter routes, then warm-start full-length optimization.
    stage_steps = sorted(
        set(
            [
                max(8, max_steps // 3),
                max(12, (2 * max_steps) // 3),
                max_steps,
            ]
        )
    )

    gens_per_restart = num_generations // num_restarts
    restart_remainder = num_generations % num_restarts

    best: Optional[dict] = None
    best_fitness = -1.0
    best_combo = -1
    final_row, final_col = 0, 0

    def _expand_center(
        prev: Optional[torch.Tensor], target_steps: int, board_size: int
    ) -> torch.Tensor:
        if prev is None:
            return torch.cat(
                [
                    torch.rand(1) * float(board_size - 1),
                    torch.rand(target_steps) * float(DIRECTION_COUNT - 1),
                ]
            ).to(dtype=torch.float32)

        out = torch.zeros(1 + target_steps, dtype=torch.float32)
        out[0] = float(prev[0].item())
        copy_steps = min(target_steps, prev.numel() - 1)
        if copy_steps > 0:
            out[1 : 1 + copy_steps] = prev[1 : 1 + copy_steps]
        if copy_steps < target_steps:
            out[1 + copy_steps :] = torch.rand(target_steps - copy_steps) * float(
                DIRECTION_COUNT - 1
            )
        return out

    for r in range(num_restarts):
        generations_this_restart = gens_per_restart + (
            1 if r < restart_remainder else 0
        )
        if generations_this_restart <= 0:
            continue

        stage_base = generations_this_restart // len(stage_steps)
        stage_remainder = generations_this_restart % len(stage_steps)
        center = None

        for s_idx, stage_max_steps in enumerate(stage_steps):
            generations_this_stage = stage_base + (1 if s_idx < stage_remainder else 0)
            if generations_this_stage <= 0:
                continue

            problem = PazusobaProblem(
                board_str,
                max_steps=stage_max_steps,
                min_erase=min_erase,
                reward_fn=reward_fn,
            )
            center = _expand_center(center, stage_max_steps, problem.board_size)
            searcher = SNES(
                problem,
                popsize=popsize,
                stdev_init=stdev_init,
                center_init=center,
            )
            if verbose:
                print(
                    f"[restart {r + 1}/{num_restarts} | "
                    f"stage {s_idx + 1}/{len(stage_steps)} steps={stage_max_steps}] "
                    f"generations={generations_this_stage}, popsize={popsize}, "
                    f"stdev_init={stdev_init}"
                )
                _ = StdOutLogger(searcher, interval=50)

            searcher.run(generations_this_stage)
            center = searcher.status["best"].values.clone()

            # Only compare final-stage solutions (same max_steps contract).
            if stage_max_steps != max_steps:
                continue

            candidate_solution = searcher.status["best"]
            candidate_fitness = float(candidate_solution.evals[0].item())
            candidate = problem.decode(candidate_solution.values)

            if candidate_fitness > best_fitness or (
                candidate_fitness == best_fitness and candidate["combo"] > best_combo
            ):
                best_fitness = candidate_fitness
                best_combo = candidate["combo"]
                best = candidate
                final_row, final_col = problem.row, problem.col

            if best is not None and best["goal"]:
                break

        if best is not None and best["goal"]:
            break

    if best is None:
        raise RuntimeError("No solution was produced by SNES.")

    # ------------------------------------------------------------------
    # Discrete local refinement: SNES works in continuous space and then
    # rounds to integers, which can plateau. This post-pass mutates integer
    # moves directly and typically improves high-combo outcomes.
    # ------------------------------------------------------------------
    if refine_iters > 0 and not best["goal"]:
        dir_to_idx = {"up": 0, "down": 1, "left": 2, "right": 3}
        idx_to_dir = ["up", "down", "left", "right"]
        initial_board, row, col = parse_board(board_str)
        board_size = row * col
        max_combo = calc_max_combo(initial_board, row, col, min_erase)
        active_reward_fn: RewardFn = (
            reward_fn if reward_fn is not None else combo_reward
        )

        def _eval_candidate(start_pos: int, dirs: List[int]) -> Tuple[int, float]:
            board_after, _ = _apply_moves(initial_board, row, col, start_pos, dirs)
            combos = count_combos(board_after, row, col, min_erase)
            # Lexicographic: combo dominates, reward breaks ties.
            shaped = active_reward_fn(combos, max_combo, board_after, row, col)
            return combos, combos + 1e-3 * shaped

        current_start = int(best["start_pos"])
        current_dirs = [dir_to_idx[d] for d in best["directions"]]
        current_combo, current_score = _eval_candidate(current_start, current_dirs)
        best_start = current_start
        best_dirs = current_dirs[:]
        best_combo_local = current_combo
        best_score_local = current_score

        # First, re-try all starts for the current direction sequence.
        for s in range(board_size):
            c_combo, c_score = _eval_candidate(s, current_dirs)
            if c_combo > best_combo_local or (
                c_combo == best_combo_local and c_score > best_score_local
            ):
                best_start = s
                best_combo_local = c_combo
                best_score_local = c_score

        current_start = best_start
        current_combo = best_combo_local
        current_score = best_score_local

        for i in range(refine_iters):
            progress = i / max(refine_iters, 1)
            temperature = max(0.02, 0.35 * (1.0 - progress))

            cand_start = current_start
            cand_dirs = current_dirs[:]

            # Mutate 1-3 positions; occasionally jump start position.
            mutation_count = (
                1 if random.random() < 0.7 else (2 if random.random() < 0.8 else 3)
            )
            for _ in range(mutation_count):
                j = random.randrange(len(cand_dirs))
                cand_dirs[j] = random.randrange(DIRECTION_COUNT)
            if random.random() < 0.08:
                cand_start = random.randrange(board_size)

            cand_combo, cand_score = _eval_candidate(cand_start, cand_dirs)

            delta = cand_score - current_score
            accept = False
            if delta >= 0:
                accept = True
            else:
                # Allow some downhill moves early for escaping local optima.
                if (
                    random.random()
                    < torch.exp(torch.tensor(delta / temperature)).item()
                ):
                    accept = True

            if accept:
                current_start = cand_start
                current_dirs = cand_dirs
                current_combo = cand_combo
                current_score = cand_score

            if current_combo > best_combo_local or (
                current_combo == best_combo_local and current_score > best_score_local
            ):
                best_start = current_start
                best_dirs = current_dirs[:]
                best_combo_local = current_combo
                best_score_local = current_score

            if best_combo_local >= max_combo:
                break

        if best_combo_local > best["combo"]:
            best = {
                "start_pos": best_start,
                "start_row": best_start // col,
                "start_col": best_start % col,
                "directions": [idx_to_dir[d] for d in best_dirs],
                "combo": best_combo_local,
                "max_combo": max_combo,
                "goal": best_combo_local >= max_combo,
            }
            final_row, final_col = row, col
            if verbose:
                print(
                    f"[refine] improved to combo={best_combo_local}/{max_combo} "
                    f"after <= {refine_iters} mutations"
                )

    # Optional explicit lookahead planner; often stronger on long routes.
    if lookahead_beam_width > 0 and not best["goal"]:
        planner_result = solve_with_lookahead(
            board_str,
            max_steps=max_steps,
            min_erase=min_erase,
            beam_width=lookahead_beam_width,
        )
        if planner_result.combo > best["combo"]:
            best = planner_result.to_dict()
            _, p_row, p_col = parse_board(board_str)
            final_row, final_col = p_row, p_col
            if verbose:
                print(
                    f"[lookahead] improved to combo={planner_result.combo}/{planner_result.max_combo} "
                    f"with beam_width={lookahead_beam_width}"
                )

    return SolveResult(
        combo=best["combo"],
        max_combo=best["max_combo"],
        start_pos=best["start_pos"],
        row=final_row,
        col=final_col,
        directions=best["directions"],
        goal=best["goal"],
    )


# ---------------------------------------------------------------------------
# Export utilities: make EvoTorch results portable in C / C++
# ---------------------------------------------------------------------------


def export_solution_json(result: SolveResult, path: str) -> None:
    """Export a :class:`SolveResult` to a JSON file.

    The output is a flat JSON object readable from C/C++ with any JSON
    library (e.g. `nlohmann/json <https://github.com/nlohmann/json>`_ or
    `cJSON <https://github.com/DaveGamble/cJSON>`_)::

        {
          "start_pos": 14,
          "start_row": 2,
          "start_col": 2,
          "directions": ["up", "right", "down", "left"],
          "combo": 5,
          "max_combo": 8,
          "goal": false
        }

    Parameters
    ----------
    result:
        The result object to serialise.
    path:
        Destination file path (e.g. ``"solution.json"``).

    """
    with open(path, "w", encoding="utf-8") as f:
        json.dump(result.to_dict(), f, indent=2)


def export_torchscript(
    network: nn.Module,
    path: str,
    board_size: int,
) -> None:
    """Export a trained policy network to a TorchScript ``.pt`` file.

    The file can be loaded and executed in C++ using
    `LibTorch <https://pytorch.org/cppdocs/>`_ with no Python dependency::

        // C++ consumer example
        auto model = torch::jit::load("policy.pt");
        model.eval();
        auto logits = model.forward({obs}).toTensor();
        auto dir    = logits.argmax(1).item<int64_t>();

    Parameters
    ----------
    network:
        Trained ``nn.Module`` (e.g. from :func:`train_general_policy`).
    path:
        Destination file path (e.g. ``"policy.pt"``).
    board_size:
        Board size the network was trained on (20, 30 or 42).

    """
    in_dim = board_size * ORB_COUNT + board_size
    example_input = torch.zeros(1, in_dim)
    traced = torch.jit.trace(network.eval(), example_input)
    traced.save(path)


def export_weights_header(
    network: nn.Module,
    path: str,
    board_size: int,
) -> None:
    """Export a trained policy as a self-contained C99 header file.

    The generated header requires **no runtime dependencies** — only
    ``<math.h>`` for ``tanhf``.  Include it in any C99 / C++ project and
    call :c:func:`pazusoba_policy_forward` at each re-observation step::

        // C / C++ consumer example
        #include "pazusoba_policy.h"

        // Build the observation vector:
        //   - board_size * 11 one-hot orb features (board_size * ORB_COUNT floats)
        //   - board_size one-hot cursor position
        float obs[PAZUSOBA_OBS_DIM] = {0};
        for (int i = 0; i < board_size; i++)
            obs[i * PAZUSOBA_ORB_COUNT + board[i]] = 1.0f;
        obs[board_size * PAZUSOBA_ORB_COUNT + cursor] = 1.0f;

        // Plan the next PAZUSOBA_LOOKAHEAD_STEPS moves at once:
        int dirs[PAZUSOBA_LOOKAHEAD_STEPS];
        pazusoba_policy_forward(obs, dirs);
        // dirs[0] = first planned move, dirs[1] = second, etc.
        // Each value: 0=up, 1=down, 2=left, 3=right

    Compile with ``-lm`` (GCC/Clang) or enable math library linking as
    required by your toolchain.

    Parameters
    ----------
    network:
        Trained ``nn.Module`` built by :func:`_build_policy`.  Must have
        exactly three ``nn.Linear`` layers (2-hidden MLP).
    path:
        Destination ``.h`` file path (e.g. ``"pazusoba_policy.h"``).
    board_size:
        Board size the network was trained on (20, 30 or 42).

    Raises
    ------
    ValueError
        If *network* does not have exactly 3 linear layers.

    """
    linear_layers = [m for m in network.modules() if isinstance(m, nn.Linear)]
    if len(linear_layers) != 3:
        raise ValueError(
            f"Expected 3 nn.Linear layers, got {len(linear_layers)}. "
            "Only networks built with _build_policy() are supported."
        )

    in_dim = board_size * ORB_COUNT + board_size
    hidden = linear_layers[0].out_features
    out_dim = linear_layers[2].out_features
    lookahead_steps = max(1, out_dim // DIRECTION_COUNT)

    def _fmt_array_1d(name: str, tensor: torch.Tensor) -> str:
        """Format a 1-D tensor as a C array literal."""
        vals = tensor.detach().float().tolist()
        items = ", ".join(f"{v:.8g}f" for v in vals)
        return f"static const float {name}[{len(vals)}] = {{{items}}};"

    def _fmt_array_2d(name: str, tensor: torch.Tensor) -> str:
        """Format a 2-D tensor as a C 2-D array literal (rows x cols)."""
        rows, cols = tensor.shape
        lines = [f"static const float {name}[{rows}][{cols}] = {{"]
        for r in range(rows):
            vals = tensor[r].detach().float().tolist()
            row_str = "    {" + ", ".join(f"{v:.8g}f" for v in vals) + "}"
            suffix = "," if r < rows - 1 else ""
            lines.append(row_str + suffix)
        lines.append("};")
        return "\n".join(lines)

    w1 = linear_layers[0].weight  # (hidden, in_dim)
    b1 = linear_layers[0].bias  # (hidden,)
    w2 = linear_layers[1].weight  # (hidden, hidden)
    b2 = linear_layers[1].bias  # (hidden,)
    w3 = linear_layers[2].weight  # (lookahead_steps*4, hidden)
    b3 = linear_layers[2].bias  # (lookahead_steps*4,)

    import os
    import re

    basename = os.path.basename(path)
    # Sanitise: replace any non-alphanumeric character with '_', ensure starts
    # with a letter so the result is always a valid C identifier.
    raw_guard = re.sub(r"[^A-Za-z0-9]", "_", basename).upper()
    if raw_guard and raw_guard[0].isdigit():
        raw_guard = "H_" + raw_guard
    guard = raw_guard

    header = textwrap.dedent(f"""\
        #pragma once
        #ifndef {guard}_
        #define {guard}_

        /*
         * Auto-generated by pazusoba evotorch_solver.py
         * Self-contained MLP policy for Puzzle & Dragons board solving.
         * Zero runtime dependencies - link with -lm (C) or compile as C++.
         *
         * Usage
         * -----
         *   #include "{basename}"
         *
         *   // Build the observation vector
         *   float obs[PAZUSOBA_OBS_DIM] = {{0}};
         *   for (int i = 0; i < PAZUSOBA_BOARD_SIZE; i++)
         *       obs[i * PAZUSOBA_ORB_COUNT + board[i]] = 1.0f;
         *   obs[PAZUSOBA_BOARD_SIZE * PAZUSOBA_ORB_COUNT + cursor] = 1.0f;
         *
         *   // Plan the next PAZUSOBA_LOOKAHEAD_STEPS moves at once
         *   int dirs[PAZUSOBA_LOOKAHEAD_STEPS];
         *   pazusoba_policy_forward(obs, dirs);
         *   // Apply dirs[0], dirs[1], ... then re-observe and call again
         *   // Each dir value: 0=up, 1=down, 2=left, 3=right
         */

        #include <math.h>

        #define PAZUSOBA_BOARD_SIZE     {board_size}
        #define PAZUSOBA_ORB_COUNT      {ORB_COUNT}
        #define PAZUSOBA_OBS_DIM        {in_dim}
        #define PAZUSOBA_HIDDEN         {hidden}
        #define PAZUSOBA_DIR_COUNT      {DIRECTION_COUNT}
        #define PAZUSOBA_LOOKAHEAD_STEPS {lookahead_steps}
        #define PAZUSOBA_OUT_DIM        {out_dim}

        """)

    header += _fmt_array_2d("pazusoba_w1", w1) + "\n"
    header += _fmt_array_1d("pazusoba_b1", b1) + "\n\n"
    header += _fmt_array_2d("pazusoba_w2", w2) + "\n"
    header += _fmt_array_1d("pazusoba_b2", b2) + "\n\n"
    header += _fmt_array_2d("pazusoba_w3", w3) + "\n"
    header += _fmt_array_1d("pazusoba_b3", b3) + "\n"

    header += textwrap.dedent("""\

        /*
         * pazusoba_policy_forward - run one planning cycle.
         *
         * Fills out_dirs[0..PAZUSOBA_LOOKAHEAD_STEPS-1] with the K planned
         * move directions for the current observation.  Execute all K moves
         * (skip invalid ones), update obs, then call this function again.
         */
        static inline void pazusoba_policy_forward(const float *obs, int *out_dirs)
        {
            float h1[PAZUSOBA_HIDDEN], h2[PAZUSOBA_HIDDEN], out[PAZUSOBA_OUT_DIM];
            int i, j, k, best;

            /* Layer 1: Linear + Tanh */
            for (i = 0; i < PAZUSOBA_HIDDEN; i++) {
                h1[i] = pazusoba_b1[i];
                for (j = 0; j < PAZUSOBA_OBS_DIM; j++)
                    h1[i] += pazusoba_w1[i][j] * obs[j];
                h1[i] = tanhf(h1[i]);
            }

            /* Layer 2: Linear + Tanh */
            for (i = 0; i < PAZUSOBA_HIDDEN; i++) {
                h2[i] = pazusoba_b2[i];
                for (j = 0; j < PAZUSOBA_HIDDEN; j++)
                    h2[i] += pazusoba_w2[i][j] * h1[j];
                h2[i] = tanhf(h2[i]);
            }

            /* Layer 3: Linear (no activation) */
            for (i = 0; i < PAZUSOBA_OUT_DIM; i++) {
                out[i] = pazusoba_b3[i];
                for (j = 0; j < PAZUSOBA_HIDDEN; j++)
                    out[i] += pazusoba_w3[i][j] * h2[j];
            }

            /* Argmax over each group of PAZUSOBA_DIR_COUNT logits */
            for (k = 0; k < PAZUSOBA_LOOKAHEAD_STEPS; k++) {
                best = 0;
                for (i = 1; i < PAZUSOBA_DIR_COUNT; i++)
                    if (out[k * PAZUSOBA_DIR_COUNT + i] > out[k * PAZUSOBA_DIR_COUNT + best])
                        best = i;
                out_dirs[k] = best;
            }
        }

        #endif /* {guard}_ */
        """)

    with open(path, "w", encoding="utf-8") as f:
        f.write(header)
