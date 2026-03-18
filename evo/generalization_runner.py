"""Generalization training/evaluation runner for EvoTorch PAD policy.

This script trains a policy on random boards and evaluates on unseen boards
from a separate split, reporting how well the policy generalizes.
"""

from __future__ import annotations

import argparse
import json
import random
import statistics
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import List

from evotorch_solver import run_policy, train_general_policy


@dataclass
class EvalRow:
    index: int
    board: str
    combo: int
    max_combo: int
    combo_ratio: float
    goal: bool
    start_pos: int
    steps: int


def make_random_boards(board_size: int, count: int, seed: int) -> List[str]:
    rng = random.Random(seed)
    # Keep generation deterministic without mutating global random state.
    return [
        "".join(rng.choice("RBGLDH") for _ in range(board_size)) for _ in range(count)
    ]


def summarize(rows: List[EvalRow]) -> dict:
    if not rows:
        return {
            "count": 0,
            "goal_rate": 0.0,
            "mean_combo_ratio": 0.0,
            "median_combo_ratio": 0.0,
            "mean_combo": 0.0,
            "mean_max_combo": 0.0,
            "best_combo_ratio": 0.0,
            "worst_combo_ratio": 0.0,
        }

    ratios = [r.combo_ratio for r in rows]
    combos = [r.combo for r in rows]
    max_combos = [r.max_combo for r in rows]
    goals = [r.goal for r in rows]

    return {
        "count": len(rows),
        "goal_rate": sum(goals) / len(goals),
        "mean_combo_ratio": statistics.fmean(ratios),
        "median_combo_ratio": statistics.median(ratios),
        "mean_combo": statistics.fmean(combos),
        "mean_max_combo": statistics.fmean(max_combos),
        "best_combo_ratio": max(ratios),
        "worst_combo_ratio": min(ratios),
    }


def run_experiment(args: argparse.Namespace) -> dict:
    train_boards = make_random_boards(args.board_size, args.train_boards, args.seed)
    valid_boards = make_random_boards(args.board_size, args.valid_boards, args.seed + 1)

    print(
        "Training policy:",
        f"board_size={args.board_size}, train_boards={args.train_boards}, "
        f"generations={args.generations}, popsize={args.popsize}, hidden={args.hidden}",
    )

    net = train_general_policy(
        board_size=args.board_size,
        num_boards=args.train_boards,
        max_steps=args.max_steps,
        min_erase=args.min_erase,
        hidden=args.hidden,
        num_starts=args.num_starts,
        popsize=args.popsize,
        num_generations=args.generations,
        verbose=args.verbose,
    )

    print(
        "Evaluating policy:",
        f"valid_boards={args.valid_boards}, max_steps={args.max_steps}, num_starts={args.num_starts}",
    )

    rows: List[EvalRow] = []
    for i, board in enumerate(valid_boards):
        result = run_policy(
            net,
            board,
            max_steps=args.max_steps,
            min_erase=args.min_erase,
            num_starts=args.num_starts,
        )
        ratio = (result.combo / result.max_combo) if result.max_combo > 0 else 0.0
        rows.append(
            EvalRow(
                index=i,
                board=board,
                combo=result.combo,
                max_combo=result.max_combo,
                combo_ratio=ratio,
                goal=result.goal,
                start_pos=result.start_pos,
                steps=len(result.directions),
            )
        )

    summary = summarize(rows)

    print("\n=== Generalization Summary ===")
    print(f"Boards evaluated      : {summary['count']}")
    print(f"Goal rate             : {summary['goal_rate']:.2%}")
    print(f"Mean combo ratio      : {summary['mean_combo_ratio']:.4f}")
    print(f"Median combo ratio    : {summary['median_combo_ratio']:.4f}")
    print(
        f"Mean combo/max_combo  : {summary['mean_combo']:.2f}/{summary['mean_max_combo']:.2f}"
    )
    print(
        f"Best ratio / Worst    : {summary['best_combo_ratio']:.4f} / {summary['worst_combo_ratio']:.4f}"
    )

    worst_rows = sorted(rows, key=lambda r: r.combo_ratio)[: args.show_worst]
    if worst_rows:
        print("\nWorst boards:")
        for row in worst_rows:
            print(
                f"  [{row.index:03d}] ratio={row.combo_ratio:.4f} "
                f"combo={row.combo}/{row.max_combo} board={row.board}"
            )

    payload = {
        "config": {
            "board_size": args.board_size,
            "train_boards": args.train_boards,
            "valid_boards": args.valid_boards,
            "max_steps": args.max_steps,
            "min_erase": args.min_erase,
            "num_starts": args.num_starts,
            "hidden": args.hidden,
            "popsize": args.popsize,
            "generations": args.generations,
            "seed": args.seed,
        },
        "summary": summary,
        "rows": [asdict(r) for r in rows],
    }

    if args.output:
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
        print(f"\nSaved report: {output_path}")

    return payload


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Train and evaluate generalization policy"
    )
    parser.add_argument("--board-size", type=int, default=30, choices=[20, 30, 42])
    parser.add_argument("--train-boards", type=int, default=50)
    parser.add_argument("--valid-boards", type=int, default=100)
    parser.add_argument("--max-steps", type=int, default=50)
    parser.add_argument("--min-erase", type=int, default=3)
    parser.add_argument("--num-starts", type=int, default=6)
    parser.add_argument("--hidden", type=int, default=64)
    parser.add_argument("--popsize", type=int, default=100)
    parser.add_argument("--generations", type=int, default=200)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--show-worst", type=int, default=5)
    parser.add_argument("--output", type=str, default="")
    parser.add_argument("--verbose", action="store_true")
    return parser.parse_args()


if __name__ == "__main__":
    run_experiment(parse_args())
