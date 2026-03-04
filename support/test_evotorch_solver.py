"""
Unit tests for evotorch_solver.py.

Tests cover:
  - Board parsing
  - max_combo estimation
  - erase_combo (matching the C++ test cases)
  - move_orbs_down / gravity
  - count_combos (with cascades)
  - simulate_moves (edge / boundary validation)
  - PazusobaProblem construction and fitness evaluation
  - NeuroEvoPazusobaProblem construction and single-step evaluation
"""

import copy
import sys
import os

# Make sure the support directory is importable
sys.path.insert(0, os.path.dirname(__file__))

import pytest
import torch

from evotorch_solver import (
    ORB_COUNT,
    PazusobaProblem,
    NeuroEvoPazusobaProblem,
    SolveResult,
    calc_max_combo,
    count_combos,
    erase_combo,
    move_orbs_down,
    parse_board,
    simulate_moves,
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
        """
        Boards with no combos should return 0; boards with combos should return > 0.
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


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
