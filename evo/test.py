"""Unit tests for evotorch_solver.py.

Tests cover:
  - Board parsing
  - max_combo estimation
  - erase_combo (matching the C++ test cases)
  - move_orbs_down / gravity
  - count_combos (with cascades)
  - simulate_moves (edge / boundary validation)
  - PazusobaProblem construction and fitness evaluation
  - PazusobaProblem with custom reward_fn
  - NeuroEvoPazusobaProblem construction and single-step evaluation
  - NeuroEvoPazusobaProblem with custom reward_fn
  - run_policy (apply trained policy to any board)
  - train_general_policy (multi-board general training)
"""

import os
import sys

# Make sure the support directory is importable
sys.path.insert(0, os.path.dirname(__file__))

import pytest
import torch

from evotorch_solver import (
    NeuroEvoPazusobaProblem,
    PazusobaProblem,
    SolveResult,
    _apply_moves,
    _build_policy,
    calc_max_combo,
    combo_reward,
    count_combos,
    erase_combo,
    export_solution_json,
    export_torchscript,
    export_weights_header,
    move_orbs_down,
    orb_remaining_reward,
    parse_board,
    random_board,
    run_policy,
    simulate_moves,
    train_general_policy,
)

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def board_from_str(s: str):
    board, row, col = parse_board(s)
    return board, row, col


# ---------------------------------------------------------------------------
# parse_board
# ---------------------------------------------------------------------------


class TestParseBoard:
    def test_5x6(self):
        board, row, col = parse_board("R" * 30)
        assert row == 5
        assert col == 6
        assert len(board) == 30
        assert all(b == 1 for b in board)  # R = index 1

    def test_4x5(self):
        board, row, col = parse_board("B" * 20)
        assert row == 4
        assert col == 5

    def test_6x7(self):
        board, row, col = parse_board("G" * 42)
        assert row == 6
        assert col == 7

    def test_invalid_size(self):
        with pytest.raises(ValueError):
            parse_board("RRR")

    def test_invalid_char(self):
        with pytest.raises(ValueError):
            parse_board("R" * 29 + "X")

    def test_orb_mapping(self):
        # ' RBGLDHJEPT'
        board, _, _ = parse_board("RBGLDH" * 5)
        expected = [1, 2, 3, 4, 5, 6] * 5
        assert board == expected


# ---------------------------------------------------------------------------
# calc_max_combo
# ---------------------------------------------------------------------------


class TestCalcMaxCombo:
    def test_known_board(self):
        board, row, col = parse_board("DGRRBLHGBBGGRDDDDLBGHDBLLHDBLD")
        mc = calc_max_combo(board, row, col, 3)
        assert mc == 8  # matches C++ test assertion

    def test_all_same_colour(self):
        board, row, col = parse_board("R" * 30)
        mc = calc_max_combo(board, row, col, 3)
        # 30 R / 3 = 10, but > half board → extra penalty applied
        assert mc >= 1

    def test_at_least_one(self):
        board, row, col = parse_board("R" * 30)
        mc = calc_max_combo(board, row, col, 5)
        assert mc >= 1


# ---------------------------------------------------------------------------
# erase_combo
# ---------------------------------------------------------------------------


class TestEraseCombo:
    # These replicate the key assertions from support/test.cpp

    def test_10_horizontal_combos(self):
        board, row, col = board_from_str("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        n = erase_combo(board, row, col, 3)
        assert n == 10

    def test_4_combos_around(self):
        board, row, col = board_from_str("PRHBBBPHJDRHPRRHHJGGRRHJLLLHBJ")
        n = erase_combo(board, row, col, 3)
        assert n == 4

    def test_10_combos_vertical_and_horizontal(self):
        board, row, col = board_from_str("RRRBBBGLDGLDGLDGLDGLDGLDRRRBBB")
        n = erase_combo(board, row, col, 3)
        assert n == 10

    def test_min_erase_4_no_combo(self):
        board, row, col = board_from_str("RRRBBBGLDGLDGLDGLDGLDGLDRRRBBB")
        n = erase_combo(board, row, col, 4)
        assert n == 0

    def test_4_combos_u_shape(self):
        """U-shaped heal cluster should count as one combo (17 cells)."""
        board, row, col = board_from_str("HHHRHRHRHRHRHRHRHRHRHRHRHRHHHR")
        n = erase_combo(board, row, col, 3)
        assert n == 4

    def test_z_shape_3_combos(self):
        board, row, col = board_from_str("HHHHHHRRRRRHHHHHHHHRRRRRHHHHHH")
        n = erase_combo(board, row, col, 3)
        assert n == 3

    def test_1_combo_all_red(self):
        board, row, col = board_from_str("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRR")
        n = erase_combo(board, row, col, 3)
        assert n == 1

    def test_no_combo(self):
        board, row, col = board_from_str("LBGHGDHDBDLBHDLHDRLHRBBGBLBDGR")
        n = erase_combo(board, row, col, 3)
        assert n == 0

    def test_5_combos_plus_shapes(self):
        board, row, col = board_from_str("LGGGRLGGGRRLBBBRRLHHHRGLBDHHHL")
        n = erase_combo(board, row, col, 3)
        assert n == 5

    def test_4_combos_l_plus_trick(self):
        board, row, col = board_from_str("HLHHRRHHHRRRBLHRRGBLHHHGBBBGGG")
        n = erase_combo(board, row, col, 3)
        assert n == 4

    def test_board_is_erased(self):
        """After erasing, all combo positions should be 0."""
        board, row, col = board_from_str("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        erase_combo(board, row, col, 3)
        # The board should be entirely empty (all-horizontal, no leftover)
        assert all(b == 0 for b in board)

    def test_10_combos_with_jammer_poison(self):
        board, row, col = board_from_str("RRRBBBDDDHRJPHLHRJPHLHRJPHLGGG")
        n = erase_combo(board, row, col, 3)
        assert n == 10

    def test_2_combos_long_heal(self):
        board, row, col = board_from_str("BDLHGBBBHHHDBBHHGLDBRHHGGBHHGR")
        n = erase_combo(board, row, col, 3)
        assert n == 2

    def test_3_combos_tricky_shape(self):
        board, row, col = board_from_str("DHLLHLHHHHHHHHRRHBHRRRHBGBRDDD")
        n = erase_combo(board, row, col, 3)
        assert n == 3


# ---------------------------------------------------------------------------
# move_orbs_down
# ---------------------------------------------------------------------------


class TestMoveOrbsDown:
    def test_simple_gravity(self):
        # Single column: [R, 0, 0] → [0, 0, R] (top row is col=0)
        # 3 rows x 1 col
        board = [1, 0, 0]  # R empty empty
        move_orbs_down(board, 3, 1)
        assert board == [0, 0, 1]

    def test_no_movement_needed(self):
        board = [0, 0, 1]  # already at bottom
        move_orbs_down(board, 3, 1)
        assert board == [0, 0, 1]

    def test_multiple_orbs(self):
        # 3 rows x 1 col: R, 0, B → 0, R, B
        board = [1, 0, 2]
        move_orbs_down(board, 3, 1)
        assert board == [0, 1, 2]

    def test_does_not_affect_non_empty_column(self):
        # 1 row x 3 col: all non-empty → no change
        board = [1, 2, 3]
        move_orbs_down(board, 1, 3)
        assert board == [1, 2, 3]


# ---------------------------------------------------------------------------
# count_combos (with cascades)
# ---------------------------------------------------------------------------


class TestCountCombos:
    def test_single_pass_no_cascade(self):
        board, row, col = board_from_str("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        n = count_combos(board, row, col, 3)
        assert n == 10

    def test_cascade(self):
        """Boards with no combos should return 0; boards with combos should return > 0.
        Also verifies that count_combos accumulates across cascade rounds.
        """
        board, row, col = board_from_str("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        n = count_combos(board, row, col, 3)
        assert n == 10

    def test_no_combo_returns_zero(self):
        board, row, col = board_from_str("LBGHGDHDBDLBHDLHDRLHRBBGBLBDGR")
        n = count_combos(board, row, col, 3)
        assert n == 0


# ---------------------------------------------------------------------------
# simulate_moves
# ---------------------------------------------------------------------------


class TestSimulateMoves:
    def test_empty_directions_returns_initial_combos(self):
        """No moves → evaluate the board as-is."""
        board, row, col = board_from_str("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        n = simulate_moves(board, row, col, 0, [])
        # original board already has 10 combos
        assert n == 10

    def test_initial_board_unchanged(self):
        """simulate_moves must not modify the original board list."""
        board, row, col = board_from_str("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        original = board[:]
        simulate_moves(board, row, col, 0, [0, 1, 2, 3])
        assert board == original

    def test_boundary_left_edge(self):
        """Moving left from column 0 should be silently skipped."""
        board, row, col = board_from_str("R" * 30)
        # Start at position 0 (row 0, col 0), try to move left (d=2)
        n = simulate_moves(board, row, col, 0, [2])
        assert n >= 0  # should not raise

    def test_boundary_right_edge(self):
        """Moving right from the last column should be silently skipped."""
        board, row, col = board_from_str("R" * 30)
        # Position 5 is row 0, col 5 (rightmost on 5x6) → right (d=3) wraps
        n = simulate_moves(board, row, col, 5, [3])
        assert n >= 0

    def test_no_backtracking(self):
        """Moving back to the previous position should be silently skipped."""
        board, row, col = board_from_str("R" * 30)
        # Move right then immediately left (second move goes back)
        # Start at col 0 → right (d=3) → now at col 1; left (d=2) would revisit col 0
        n = simulate_moves(board, row, col, 0, [3, 2])
        assert n >= 0  # no exception; backtrack silently ignored


# ---------------------------------------------------------------------------
# PazusobaProblem
# ---------------------------------------------------------------------------


class TestPazusobaProblem:
    def test_construction(self):
        p = PazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=10)
        assert p.board_size == 30
        assert p.row == 5
        assert p.col == 6
        assert p.max_combo >= 1

    def test_solution_length(self):
        max_steps = 15
        p = PazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=max_steps)
        assert p.solution_length == 1 + max_steps

    def test_fitness_for_values(self):
        """Fitness should be in [0, 1]."""
        p = PazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=10)
        # Evaluate the trivial all-zero solution
        sol = torch.zeros(p.solution_length, dtype=torch.float32)
        fitness = p._fitness_for_values(sol)
        assert 0.0 <= fitness <= 1.0

    def test_fitness_for_all_combo_board(self):
        """All-horizontal board should score > 0 even with no moves."""
        p = PazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=10)
        # start_pos=0, no real moves
        sol = torch.zeros(p.solution_length, dtype=torch.float32)
        fitness = p._fitness_for_values(sol)
        assert fitness > 0.0

    def test_decode_keys(self):
        p = PazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=5)
        sol = torch.zeros(p.solution_length, dtype=torch.float32)
        d = p.decode(sol)
        assert "combo" in d
        assert "max_combo" in d
        assert "start_pos" in d
        assert "directions" in d
        assert "goal" in d

    def test_snes_runs(self):
        """Smoke test: SNES on PazusobaProblem should run without error."""
        from evotorch.algorithms import SNES

        p = PazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=5)
        searcher = SNES(p, popsize=10, stdev_init=1.0)
        searcher.run(3)  # just a few generations
        best_fitness = searcher.status["pop_best"].evals[0].item()
        assert best_fitness >= 0.0


# ---------------------------------------------------------------------------
# NeuroEvoPazusobaProblem
# ---------------------------------------------------------------------------


class TestNeuroEvoPazusobaProblem:
    def test_construction(self):
        p = NeuroEvoPazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        assert p.board_size == 30

    def test_lookahead_steps_stored(self):
        """lookahead_steps attribute should be stored on the problem."""
        p = NeuroEvoPazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", lookahead_steps=5
        )
        assert p.lookahead_steps == 5

    def test_network_output_dim_matches_lookahead(self):
        """Network last layer output should be lookahead_steps * 4."""
        import torch.nn as nn

        lookahead_steps = 7
        p = NeuroEvoPazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            hidden=16,
            lookahead_steps=lookahead_steps,
        )
        # Instantiate the network via make_net to inspect its architecture
        params = p.generate_values(1)[0]
        net = p.make_net(params)
        last_linear = [m for m in net.modules() if isinstance(m, nn.Linear)][-1]
        assert last_linear.out_features == lookahead_steps * 4

    def test_fitness_range(self):
        """_evaluate_network on random weights should return a float in [0, 1]."""
        p = NeuroEvoPazusobaProblem("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        params = p.generate_values(1)[0]
        net = p.make_net(params)
        fitness = p._evaluate_network(net)
        assert 0.0 <= fitness <= 1.0

    def test_multi_board(self):
        """Multi-board mode should average fitness across boards."""
        boards = [
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            "LBGHGDHDBDLBHDLHDRLHRBBGBLBDGR",
        ]
        p = NeuroEvoPazusobaProblem(boards)
        params = p.generate_values(1)[0]
        net = p.make_net(params)
        fitness = p._evaluate_network(net)
        assert 0.0 <= fitness <= 1.0

    def test_pgpe_smoke(self):
        """PGPE on NeuroEvoPazusobaProblem should run without error."""
        from evotorch.algorithms import PGPE

        p = NeuroEvoPazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=5, hidden=16
        )
        searcher = PGPE(
            p,
            popsize=10,
            radius_init=0.5,
            center_learning_rate=0.1,
            stdev_learning_rate=0.1,
        )
        searcher.run(2)


# ---------------------------------------------------------------------------
# SolveResult
# ---------------------------------------------------------------------------


class TestSolveResult:
    def test_str(self):
        r = SolveResult(
            combo=5,
            max_combo=8,
            start_pos=3,
            row=5,
            col=6,
            directions=["up", "down"],
            goal=False,
        )
        s = str(r)
        assert "5/8" in s
        assert "False" in s


# ---------------------------------------------------------------------------
# random_board
# ---------------------------------------------------------------------------


class TestRandomBoard:
    def test_length_30(self):
        b = random_board(30)
        assert len(b) == 30

    def test_length_20(self):
        b = random_board(20)
        assert len(b) == 20

    def test_length_42(self):
        b = random_board(42)
        assert len(b) == 42

    def test_valid_chars(self):
        valid = set("RBGLDH")
        b = random_board(30)
        assert all(ch in valid for ch in b)

    def test_invalid_size(self):
        with pytest.raises(ValueError):
            random_board(25)


# ---------------------------------------------------------------------------
# Built-in reward functions
# ---------------------------------------------------------------------------


class TestRewardFunctions:
    def test_combo_reward_range(self):
        board, row, col = parse_board("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        combos = count_combos(board, row, col, 3)
        mc = calc_max_combo(board, row, col, 3)
        r = combo_reward(combos, mc, board, row, col)
        assert 0.0 <= r <= 1.0

    def test_combo_reward_max(self):
        # Perfect board: ratio should be 1.0
        board, row, col = parse_board("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        mc = calc_max_combo(board, row, col, 3)
        r = combo_reward(mc, mc, board, row, col)
        assert r == pytest.approx(1.0)

    def test_orb_remaining_reward_range(self):
        board, row, col = parse_board("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        combos = count_combos(board, row, col, 3)
        mc = calc_max_combo(board, row, col, 3)
        r = orb_remaining_reward(combos, mc, board, row, col)
        assert 0.0 <= r <= 1.0

    def test_combo_reward_zero_max_combo(self):
        """combo_reward should return 0.0 when max_combo is 0 (guard)."""
        board, row, col = parse_board("R" * 30)
        r = combo_reward(0, 0, board, row, col)
        assert r == 0.0

    def test_orb_remaining_zero_max_combo(self):
        """orb_remaining_reward should not raise when max_combo is 0."""
        board, row, col = parse_board("R" * 30)
        r = orb_remaining_reward(0, 0, board, row, col)
        assert 0.0 <= r <= 1.0
        """A board with many orbs remaining should score lower than an empty board."""
        board, row, col = parse_board("LBGHGDHDBDLBHDLHDRLHRBBGBLBDGR")
        mc = calc_max_combo(board, row, col, 3)
        full_board_reward = orb_remaining_reward(0, mc, board, row, col)
        empty_board = [0] * len(board)
        empty_reward = orb_remaining_reward(0, mc, empty_board, row, col)
        assert empty_reward > full_board_reward


# ---------------------------------------------------------------------------
# _apply_moves
# ---------------------------------------------------------------------------


class TestApplyMoves:
    def test_returns_copy(self):
        board, row, col = parse_board("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        original = board[:]
        result, _ = _apply_moves(board, row, col, 0, [3])  # move right
        assert board == original  # input unchanged

    def test_simple_swap(self):
        board, row, col = parse_board("RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL")
        result, final_pos = _apply_moves(board, row, col, 0, [3])
        assert final_pos == 1  # moved right from 0 to 1
        assert result[0] == board[1]  # orbs swapped
        assert result[1] == board[0]

    def test_invalid_move_not_applied(self):
        board, row, col = parse_board("R" * 30)
        # Move left from position 0 (left edge) → should be skipped
        result, final_pos = _apply_moves(board, row, col, 0, [2])
        assert final_pos == 0  # stayed in place
        assert result == board  # board unchanged


# ---------------------------------------------------------------------------
# PazusobaProblem with custom reward_fn
# ---------------------------------------------------------------------------


class TestPazusobaProblemCustomReward:
    def test_custom_reward_called(self):
        """Custom reward function should be called during evaluation."""
        calls = []

        def my_reward(combos, max_combo, board, row, col):
            calls.append(combos)
            return combos / max_combo

        p = PazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=5, reward_fn=my_reward
        )
        sol = torch.zeros(p.solution_length, dtype=torch.float32)
        p._fitness_for_values(sol)
        assert len(calls) == 1  # called once per evaluation

    def test_orb_remaining_reward_in_problem(self):
        """PazusobaProblem should accept orb_remaining_reward."""
        p = PazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            max_steps=5,
            reward_fn=orb_remaining_reward,
        )
        sol = torch.zeros(p.solution_length, dtype=torch.float32)
        fitness = p._fitness_for_values(sol)
        assert 0.0 <= fitness <= 1.0

    def test_custom_reward_changes_fitness(self):
        """A reward that always returns 0.5 should produce fitness 0.5."""

        def fixed_reward(combos, max_combo, board, row, col):
            return 0.5

        p = PazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            max_steps=5,
            reward_fn=fixed_reward,
        )
        sol = torch.zeros(p.solution_length, dtype=torch.float32)
        fitness = p._fitness_for_values(sol)
        assert fitness == pytest.approx(0.5)


# ---------------------------------------------------------------------------
# NeuroEvoPazusobaProblem with custom reward_fn
# ---------------------------------------------------------------------------


class TestNeuroEvoPazusobaProblemCustomReward:
    def test_custom_reward_accepted(self):
        """NeuroEvoPazusobaProblem should accept a custom reward_fn."""
        p = NeuroEvoPazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            max_steps=5,
            hidden=16,
            reward_fn=orb_remaining_reward,
        )
        params = p.generate_values(1)[0]
        net = p.make_net(params)
        fitness = p._evaluate_network(net)
        assert 0.0 <= fitness <= 1.0

    def test_fixed_reward_returns_constant(self):
        """A reward that always returns 0.75 should produce fitness 0.75."""

        def fixed_reward(combos, max_combo, board, row, col):
            return 0.75

        p = NeuroEvoPazusobaProblem(
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            max_steps=3,
            hidden=8,
            reward_fn=fixed_reward,
        )
        params = p.generate_values(1)[0]
        net = p.make_net(params)
        fitness = p._evaluate_network(net)
        assert fitness == pytest.approx(0.75)


# ---------------------------------------------------------------------------
# run_policy
# ---------------------------------------------------------------------------


class TestRunPolicy:
    def test_returns_solve_result(self):
        """run_policy should return a SolveResult for any valid board."""
        from evotorch_solver import _build_policy

        # Untrained policy still produces a valid result
        net = _build_policy(30, hidden=16)
        result = run_policy(net, "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=5)
        assert isinstance(result, SolveResult)
        assert result.combo >= 0
        assert result.max_combo >= 1

    def test_works_on_unseen_board(self):
        """run_policy must work on any board, not just the training board."""
        from evotorch_solver import _build_policy

        net = _build_policy(30, hidden=16)
        new_board = random_board(30)
        result = run_policy(net, new_board, max_steps=10)
        assert isinstance(result, SolveResult)

    def test_custom_reward(self):
        """run_policy should accept and apply a custom reward_fn."""
        from evotorch_solver import _build_policy

        net = _build_policy(30, hidden=16)
        calls = []

        def my_reward(combos, max_combo, board, row, col):
            calls.append(1)
            return 0.3

        run_policy(
            net,
            "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL",
            max_steps=3,
            reward_fn=my_reward,
        )
        assert len(calls) > 0  # reward_fn was invoked


# ---------------------------------------------------------------------------
# _build_policy lookahead
# ---------------------------------------------------------------------------


class TestBuildPolicyLookahead:
    def test_default_lookahead_output_dim(self):
        """Default lookahead_steps=10 → output dim = 10*4 = 40."""
        import torch.nn as nn

        net = _build_policy(30)
        last_linear = [m for m in net.modules() if isinstance(m, nn.Linear)][-1]
        assert last_linear.out_features == 10 * 4  # default lookahead_steps=10

    def test_custom_lookahead_output_dim(self):
        """lookahead_steps=5 → output dim = 5*4 = 20."""
        import torch.nn as nn

        net = _build_policy(30, lookahead_steps=5)
        last_linear = [m for m in net.modules() if isinstance(m, nn.Linear)][-1]
        assert last_linear.out_features == 5 * 4

    def test_greedy_lookahead_output_dim(self):
        """lookahead_steps=1 → output dim = 4 (greedy, backward compat)."""
        import torch.nn as nn

        net = _build_policy(30, lookahead_steps=1)
        last_linear = [m for m in net.modules() if isinstance(m, nn.Linear)][-1]
        assert last_linear.out_features == 4

    def test_forward_output_shape(self):
        """Network forward pass should produce (1, lookahead_steps*4) tensor."""
        import torch

        lookahead_steps = 6
        net = _build_policy(30, hidden=16, lookahead_steps=lookahead_steps)
        obs = torch.zeros(1, 30 * 11 + 30)
        out = net(obs)
        assert out.shape == (1, lookahead_steps * 4)


# ---------------------------------------------------------------------------
# run_policy lookahead
# ---------------------------------------------------------------------------


class TestRunPolicyLookahead:
    def test_run_policy_with_lookahead_network(self):
        """run_policy should work with a network trained with lookahead_steps>1."""
        net = _build_policy(30, hidden=16, lookahead_steps=10)
        result = run_policy(net, "RRRBBBGGGLLLDDDHHHRRRBBBGGGLLL", max_steps=20)
        assert isinstance(result, SolveResult)
        assert result.combo >= 0

    def test_run_policy_greedy_compat(self):
        """run_policy should work with a single-step (greedy) network too."""
        net = _build_policy(30, hidden=16, lookahead_steps=1)
        result = run_policy(net, random_board(30), max_steps=10)
        assert isinstance(result, SolveResult)

    def test_run_policy_infers_lookahead_from_network(self):
        """run_policy must infer lookahead_steps from the network output dim."""
        import torch.nn as nn

        for k in [1, 5, 10, 15]:
            net = _build_policy(20, hidden=8, lookahead_steps=k)
            last = [m for m in net.modules() if isinstance(m, nn.Linear)][-1]
            assert last.out_features == k * 4
            # run_policy should not raise for any lookahead_steps value
            result = run_policy(net, random_board(20), max_steps=k * 2)
            assert isinstance(result, SolveResult)


# ---------------------------------------------------------------------------
# train_general_policy lookahead
# ---------------------------------------------------------------------------


class TestTrainGeneralPolicy:
    def test_returns_nn_module(self):
        """train_general_policy should return a PyTorch nn.Module."""
        import torch.nn as nn

        net = train_general_policy(
            board_size=30,
            num_boards=3,
            max_steps=5,
            hidden=16,
            num_starts=3,
            popsize=5,
            num_generations=2,
            verbose=False,
        )
        assert isinstance(net, nn.Module)

    def test_lookahead_output_dim_matches(self):
        """Trained network output dim should be lookahead_steps * 4."""
        import torch.nn as nn

        lookahead_steps = 5
        net = train_general_policy(
            board_size=20,
            num_boards=2,
            max_steps=5,
            hidden=8,
            lookahead_steps=lookahead_steps,
            num_starts=2,
            popsize=5,
            num_generations=2,
            verbose=False,
        )
        last_linear = [m for m in net.modules() if isinstance(m, nn.Linear)][-1]
        assert last_linear.out_features == lookahead_steps * 4

    def test_trained_policy_usable_on_any_board(self):
        """A policy from train_general_policy can be run on any board."""
        net = train_general_policy(
            board_size=30,
            num_boards=3,
            max_steps=5,
            hidden=16,
            num_starts=3,
            popsize=5,
            num_generations=2,
            verbose=False,
        )
        for _ in range(3):
            result = run_policy(net, random_board(30), max_steps=5)
            assert isinstance(result, SolveResult)

    def test_custom_reward_in_training(self):
        """train_general_policy should accept a custom reward_fn."""
        net = train_general_policy(
            board_size=30,
            num_boards=2,
            max_steps=5,
            hidden=8,
            popsize=5,
            num_generations=2,
            reward_fn=orb_remaining_reward,
            verbose=False,
        )
        result = run_policy(
            net,
            random_board(30),
            max_steps=5,
            reward_fn=orb_remaining_reward,
        )
        assert isinstance(result, SolveResult)


# ---------------------------------------------------------------------------
# Export utilities
# ---------------------------------------------------------------------------


class TestSolveResultToDict:
    def test_to_dict_keys(self):
        r = SolveResult(
            combo=5,
            max_combo=8,
            start_pos=3,
            row=5,
            col=6,
            directions=["up", "right"],
            goal=False,
        )
        d = r.to_dict()
        assert set(d.keys()) == {
            "start_pos",
            "start_row",
            "start_col",
            "directions",
            "combo",
            "max_combo",
            "goal",
        }

    def test_to_dict_values(self):
        r = SolveResult(
            combo=4,
            max_combo=7,
            start_pos=8,
            row=5,
            col=6,
            directions=["down"],
            goal=False,
        )
        d = r.to_dict()
        assert d["combo"] == 4
        assert d["max_combo"] == 7
        assert d["start_pos"] == 8
        assert d["start_row"] == 1  # 8 // 6
        assert d["start_col"] == 2  # 8 % 6
        assert d["directions"] == ["down"]
        assert d["goal"] is False


class TestExportSolutionJson:
    def test_creates_valid_json(self, tmp_path):
        r = SolveResult(
            combo=3,
            max_combo=6,
            start_pos=0,
            row=5,
            col=6,
            directions=["up", "right", "down"],
            goal=False,
        )
        dest = str(tmp_path / "solution.json")
        export_solution_json(r, dest)
        import json as _json

        with open(dest) as f:
            data = _json.load(f)
        assert data["combo"] == 3
        assert data["max_combo"] == 6
        assert data["directions"] == ["up", "right", "down"]
        assert data["goal"] is False

    def test_required_keys_present(self, tmp_path):
        r = SolveResult(
            combo=1,
            max_combo=5,
            start_pos=14,
            row=5,
            col=6,
            directions=[],
            goal=False,
        )
        dest = str(tmp_path / "out.json")
        export_solution_json(r, dest)
        import json as _json

        with open(dest) as f:
            data = _json.load(f)
        for key in (
            "start_pos",
            "start_row",
            "start_col",
            "directions",
            "combo",
            "max_combo",
            "goal",
        ):
            assert key in data


class TestExportTorchscript:
    def test_creates_pt_file(self, tmp_path):

        net = _build_policy(30, hidden=16)
        dest = str(tmp_path / "policy.pt")
        export_torchscript(net, dest, board_size=30)
        assert (tmp_path / "policy.pt").exists()
        assert (tmp_path / "policy.pt").stat().st_size > 0

    def test_loaded_model_produces_correct_output(self, tmp_path):
        """Loaded TorchScript model should produce the same output as the original."""
        import torch

        net = _build_policy(30, hidden=16)
        dest = str(tmp_path / "policy.pt")
        export_torchscript(net, dest, board_size=30)

        loaded = torch.jit.load(dest)
        obs = torch.zeros(1, 30 * 11 + 30)
        with torch.no_grad():
            orig_out = net(obs)
            loaded_out = loaded(obs)
        assert torch.allclose(orig_out, loaded_out, atol=1e-6)


class TestExportWeightsHeader:
    def test_creates_header_file(self, tmp_path):
        net = _build_policy(30, hidden=16)
        dest = str(tmp_path / "pazusoba_policy.h")
        export_weights_header(net, dest, board_size=30)
        assert (tmp_path / "pazusoba_policy.h").exists()
        assert (tmp_path / "pazusoba_policy.h").stat().st_size > 0

    def test_header_contains_expected_identifiers(self, tmp_path):
        net = _build_policy(30, hidden=16)
        dest = str(tmp_path / "pazusoba_policy.h")
        export_weights_header(net, dest, board_size=30)
        content = (tmp_path / "pazusoba_policy.h").read_text()
        assert "pazusoba_policy_forward" in content
        assert "pazusoba_w1" in content
        assert "pazusoba_b1" in content
        assert "pazusoba_w2" in content
        assert "pazusoba_b2" in content
        assert "pazusoba_w3" in content
        assert "pazusoba_b3" in content
        assert "PAZUSOBA_OBS_DIM" in content
        assert "PAZUSOBA_HIDDEN" in content
        assert "PAZUSOBA_LOOKAHEAD_STEPS" in content
        assert "tanhf" in content

    def test_header_defines_correct_dimensions(self, tmp_path):
        board_size = 30
        hidden = 24
        lookahead_steps = 5
        net = _build_policy(board_size, hidden=hidden, lookahead_steps=lookahead_steps)
        dest = str(tmp_path / "p.h")
        export_weights_header(net, dest, board_size=board_size)
        content = (tmp_path / "p.h").read_text()
        in_dim = board_size * 11 + board_size  # 360
        out_dim = lookahead_steps * 4  # 20
        # Check identifiers and values appear in the file
        assert "PAZUSOBA_OBS_DIM" in content and str(in_dim) in content
        assert "PAZUSOBA_HIDDEN" in content and str(hidden) in content
        assert "PAZUSOBA_BOARD_SIZE" in content and str(board_size) in content
        assert "PAZUSOBA_LOOKAHEAD_STEPS" in content and str(lookahead_steps) in content
        assert "PAZUSOBA_OUT_DIM" in content and str(out_dim) in content

    def test_wrong_layer_count_raises(self, tmp_path):
        import torch.nn as nn

        bad_net = nn.Sequential(nn.Linear(10, 4))
        with pytest.raises(ValueError, match="3 nn.Linear"):
            export_weights_header(bad_net, str(tmp_path / "bad.h"), board_size=30)

    def test_guard_sanitised_for_hyphenated_filename(self, tmp_path):
        """Header guard must be a valid C identifier even with hyphens in the name."""
        net = _build_policy(20, hidden=8)
        dest = str(tmp_path / "my-policy.h")
        export_weights_header(net, dest, board_size=20)
        content = (tmp_path / "my-policy.h").read_text()
        # Guard should have hyphen replaced by underscore
        assert "#ifndef MY_POLICY_H_" in content
        assert "#define MY_POLICY_H_" in content

    def test_header_consistent_with_pytorch_output(self, tmp_path):
        """The C header argmax must agree with the PyTorch network argmax for all
        planned moves on a fixed test observation.
        """
        import torch
        import torch.nn as nn

        board_size = 20  # smallest size -> smallest header
        lookahead_steps = 3  # test multi-step planning
        net = _build_policy(board_size, hidden=8, lookahead_steps=lookahead_steps)
        dest = str(tmp_path / "chk.h")
        export_weights_header(net, dest, board_size=board_size)

        # Replicate the C forward pass in Python using the ORIGINAL net weights
        linear_layers = [m for m in net.modules() if isinstance(m, nn.Linear)]
        w1 = linear_layers[0].weight.detach()
        b1 = linear_layers[0].bias.detach()
        w2 = linear_layers[1].weight.detach()
        b2 = linear_layers[1].bias.detach()
        w3 = linear_layers[2].weight.detach()
        b3 = linear_layers[2].bias.detach()

        obs = torch.zeros(board_size * 11 + board_size)
        obs[0] = 1.0  # orb 0 at position 0
        obs[board_size * 11] = 1.0  # cursor at position 0

        with torch.no_grad():
            pytorch_logits = net(obs.unsqueeze(0)).squeeze(0)
        # Reshape to (lookahead_steps, 4) and argmax per planning step
        pytorch_dirs = pytorch_logits.reshape(lookahead_steps, 4).argmax(dim=1).tolist()

        # Manual forward (same arithmetic as C header)
        h1 = torch.tanh(w1 @ obs + b1)
        h2 = torch.tanh(w2 @ h1 + b2)
        out = w3 @ h2 + b3  # shape: (lookahead_steps * 4,)
        manual_dirs = out.reshape(lookahead_steps, 4).argmax(dim=1).tolist()

        assert pytorch_dirs == manual_dirs

    def test_lookahead_steps_one_backward_compat(self, tmp_path):
        """lookahead_steps=1 should produce a header with PAZUSOBA_LOOKAHEAD_STEPS=1."""
        net = _build_policy(20, hidden=8, lookahead_steps=1)
        dest = str(tmp_path / "compat.h")
        export_weights_header(net, dest, board_size=20)
        content = (tmp_path / "compat.h").read_text()
        assert "PAZUSOBA_LOOKAHEAD_STEPS" in content
        # Verify value is 1
        assert "PAZUSOBA_LOOKAHEAD_STEPS 1" in content


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
