"""
EvoTorch-based optimizer for Puzzle & Dragons board solving.

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
    searcher = PGPE(problem, popsize=100, radius_init=2.25, center_learning_rate=0.2)
    logger = StdOutLogger(searcher, interval=10)
    searcher.run(200)
"""

from __future__ import annotations

import copy
from typing import List, Optional, Tuple

import torch
import torch.nn as nn
from evotorch import Problem
from evotorch.algorithms import SNES
from evotorch.neuroevolution import NEProblem
from evotorch.logging import StdOutLogger

# ---------------------------------------------------------------------------
# Orb constants (matching C++ ORB_WEB_NAME)
# ---------------------------------------------------------------------------

ORB_WEB_NAME = " RBGLDHJEPT"  # index → char
ORB_COUNT = 11

# Direction indices: 0=up, 1=down, 2=left, 3=right (same order as C++)
DIRECTION_COUNT = 4

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
    """
    Erase all matchable combos from *board* (in-place).

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
                    if (
                        in_combo[nidx]
                        and not visited[nidx]
                        and board[nidx] == orb_type
                    ):
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
    """
    Count total combos (including cascades after gravity) for a board state.

    Modifies *board* in-place (mirrors the C++ evaluate loop).
    """
    board_copy = board[:]
    total = 0
    while True:
        new_combos = erase_combo(board_copy, row, col, min_erase)
        if new_combos == 0:
            break
        total += new_combos
        move_orbs_down(board_copy, row, col)
    return total


def simulate_moves(
    initial_board: List[int],
    row: int,
    col: int,
    start_pos: int,
    directions: List[int],
    min_erase: int = 3,
) -> int:
    """
    Simulate a move sequence on the board and return the combo count.

    *directions* is a list of direction indices (0=up, 1=down, 2=left, 3=right).
    Invalid moves (boundary violations, moving back to previous position) are
    silently skipped, matching the C++ expand() logic.
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

        # Swap orbs
        board[curr], board[nxt] = board[nxt], board[curr]
        prev = curr
        curr = nxt

    return count_combos(board, row, col, min_erase)


# ---------------------------------------------------------------------------
# EvoTorch problem: direct move-sequence optimisation
# ---------------------------------------------------------------------------


class PazusobaProblem(Problem):
    """
    EvoTorch Problem that evolves a fixed-length move sequence for a given board.

    A solution is encoded as a float32 tensor of length ``1 + max_steps``:
      - ``x[0]``        : starting board position (clamped to [0, board_size-1])
      - ``x[1..N]``     : move directions (rounded to integers in [0, 3])

    The fitness is ``combo_count / max_combo`` ∈ [0, 1].
    """

    def __init__(
        self,
        board_str: str,
        max_steps: int = 50,
        min_erase: int = 3,
    ) -> None:
        self.board_str = board_str
        self.max_steps = max_steps
        self.min_erase = min_erase

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
    def _fitness_for_values(self, x: torch.Tensor) -> float:
        """Return fitness for a single solution tensor *x*."""
        vals = x.numpy()
        start_pos = int(round(float(vals[0])))
        start_pos = max(0, min(start_pos, self.board_size - 1))
        directions = [
            max(0, min(int(round(float(v))), DIRECTION_COUNT - 1))
            for v in vals[1:]
        ]
        combos = simulate_moves(
            self.initial_board,
            self.row,
            self.col,
            start_pos,
            directions,
            self.min_erase,
        )
        return combos / self.max_combo

    def _evaluate(self, solution) -> None:
        fitness = self._fitness_for_values(solution.values)
        solution.set_evals(torch.tensor([fitness], dtype=torch.float32))

    # ------------------------------------------------------------------
    def decode(self, solution_values: torch.Tensor) -> dict:
        """Decode a solution tensor into a human-readable dict."""
        vals = solution_values.numpy()
        start_pos = int(round(float(vals[0])))
        start_pos = max(0, min(start_pos, self.board_size - 1))
        dir_names = ["up", "down", "left", "right"]
        directions = [
            dir_names[max(0, min(int(round(float(v))), 3))] for v in vals[1:]
        ]
        int_dirs = [max(0, min(int(round(float(v))), 3)) for v in vals[1:]]
        combos = simulate_moves(
            self.initial_board,
            self.row,
            self.col,
            start_pos,
            int_dirs,
            self.min_erase,
        )
        return {
            "start_pos": start_pos,
            "start_row": start_pos // self.col,
            "start_col": start_pos % self.col,
            "directions": directions,
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


def _build_policy(board_size: int, hidden: int = 64) -> nn.Module:
    in_dim = board_size * ORB_COUNT + board_size
    return nn.Sequential(
        nn.Linear(in_dim, hidden),
        nn.Tanh(),
        nn.Linear(hidden, hidden),
        nn.Tanh(),
        nn.Linear(hidden, DIRECTION_COUNT),
    )


class NeuroEvoPazusobaProblem(NEProblem):
    """
    EvoTorch NEProblem that evolves the weights of a small MLP policy.

    The policy observes the current board state + cursor position and outputs
    a move direction at each step.  Fitness is ``combo_count / max_combo``.
    This enables the policy to *generalise* across multiple board layouts.

    Parameters
    ----------
    board_str:
        The board to optimise on (or a list of boards for multi-board training).
    max_steps:
        Maximum number of moves per episode.
    min_erase:
        Minimum orbs required to form a combo.
    hidden:
        Hidden layer size of the policy network.
    """

    def __init__(
        self,
        board_str: str | List[str],
        max_steps: int = 50,
        min_erase: int = 3,
        hidden: int = 64,
    ) -> None:
        self.min_erase = min_erase
        self.max_steps = max_steps

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
        self._policy_template = _build_policy(board_size, hidden)

        super().__init__(
            objective_sense="max",
            network=_build_policy(board_size, hidden),
            num_actors=1,
        )

    # ------------------------------------------------------------------
    def _observe(
        self, board: List[int], board_size: int, curr: int
    ) -> torch.Tensor:
        """Encode board state + cursor position as a flat float tensor."""
        # One-hot encode each orb
        board_feat = torch.zeros(board_size * ORB_COUNT)
        for i, orb in enumerate(board):
            board_feat[i * ORB_COUNT + orb] = 1.0
        # One-hot encode cursor position
        pos_feat = torch.zeros(board_size)
        pos_feat[curr] = 1.0
        return torch.cat([board_feat, pos_feat])

    def _evaluate_network(self, network: nn.Module) -> float:
        """Evaluate network on all boards and return mean normalised combo."""
        total = 0.0
        for initial_board, row, col, max_combo in self._boards:
            board_size = row * col
            dir_offsets = [-col, col, -1, 1]

            # Try each starting position and keep best result
            best_ratio = 0.0
            for start in range(board_size):
                board = initial_board[:]
                curr = start
                prev = start

                for _ in range(self.max_steps):
                    obs = self._observe(board, board_size, curr)
                    with torch.no_grad():
                        logits = network(obs.unsqueeze(0)).squeeze(0)
                    d = int(torch.argmax(logits).item())
                    offset = dir_offsets[d]
                    nxt = curr + offset

                    # Validate move
                    if nxt == prev or nxt < 0 or nxt >= board_size:
                        continue
                    if d == 3 and nxt % col == 0:
                        continue
                    if d == 2 and curr % col == 0:
                        continue

                    board[curr], board[nxt] = board[nxt], board[curr]
                    prev = curr
                    curr = nxt

                combos = count_combos(board, row, col, self.min_erase)
                ratio = combos / max_combo
                if ratio > best_ratio:
                    best_ratio = ratio

            total += best_ratio
        return total / len(self._boards)


# ---------------------------------------------------------------------------
# Convenience solve() function
# ---------------------------------------------------------------------------


class SolveResult:
    """Lightweight result object returned by :func:`solve`."""

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
    verbose: bool = True,
) -> SolveResult:
    """
    Use EvoTorch (SNES) to find an optimal move sequence for *board_str*.

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
        Number of generations to run.
    verbose:
        Print progress to stdout.

    Returns
    -------
    SolveResult
        The best solution found.
    """
    problem = PazusobaProblem(board_str, max_steps=max_steps, min_erase=min_erase)
    searcher = SNES(problem, popsize=popsize, stdev_init=1.0)
    if verbose:
        _ = StdOutLogger(searcher, interval=50)

    searcher.run(num_generations)

    best = problem.decode(searcher.status["pop_best"].values)

    return SolveResult(
        combo=best["combo"],
        max_combo=best["max_combo"],
        start_pos=best["start_pos"],
        row=problem.row,
        col=problem.col,
        directions=best["directions"],
        goal=best["goal"],
    )


# ---------------------------------------------------------------------------
# CLI entry point
# ---------------------------------------------------------------------------

if __name__ == "__main__":
    import sys
    import time

    board = (
        sys.argv[1] if len(sys.argv) > 1 else "LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL"
    )
    max_steps = int(sys.argv[2]) if len(sys.argv) > 2 else 50
    generations = int(sys.argv[3]) if len(sys.argv) > 3 else 500

    print(f"Board: {board}")
    print(f"Max steps: {max_steps}, Generations: {generations}")
    t0 = time.time()
    result = solve(board, max_steps=max_steps, num_generations=generations)
    print(f"\nResult:\n{result}")
    print(f"Time: {time.time() - t0:.2f}s")
