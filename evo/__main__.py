from __future__ import annotations

import argparse
import time

from evotorch_solver import run_policy, solve, train_general_policy


def _cmd_solve(args: argparse.Namespace) -> int:
    print(f"Board: {args.board}")
    print(
        f"Max steps: {args.max_steps}, Generations: {args.generations}, "
        f"Beam size: {args.beam_size}"
    )

    t0 = time.time()
    result = solve(
        args.board,
        max_steps=args.max_steps,
        min_erase=args.min_erase,
        popsize=args.popsize,
        num_generations=args.generations,
        num_restarts=args.restarts,
        stdev_init=args.stdev_init,
        refine_iters=args.refine_iters,
        lookahead_beam_width=args.beam_size,
        verbose=not args.quiet,
    )
    print(f"\nResult:\n{result}")
    print(f"Time: {time.time() - t0:.2f}s")
    return 0


def _cmd_train(args: argparse.Namespace) -> int:
    print(
        f"Training general policy: board_size={args.board_size}, "
        f"num_boards={args.num_boards}, generations={args.generations}, "
        f"lookahead_steps={args.lookahead_steps}, "
        f"boards_per_generation={args.boards_per_generation}"
    )

    t0 = time.time()
    net = train_general_policy(
        board_size=args.board_size,
        num_boards=args.num_boards,
        max_steps=args.max_steps,
        min_erase=args.min_erase,
        hidden=args.hidden,
        lookahead_steps=args.lookahead_steps,
        num_starts=args.num_starts,
        popsize=args.popsize,
        num_generations=args.generations,
        boards_per_generation=args.boards_per_generation,
        verbose=not args.quiet,
    )

    if args.eval_board:
        eval_result = run_policy(
            net,
            args.eval_board,
            max_steps=args.max_steps,
            min_erase=args.min_erase,
            num_starts=args.num_starts,
        )
        print("\nEvaluation board result:")
        print(eval_result)

    if args.export_torchscript:
        from evotorch_solver import export_torchscript

        export_torchscript(net, args.export_torchscript, board_size=args.board_size)
        print(f"Saved TorchScript policy: {args.export_torchscript}")

    if args.export_header:
        from evotorch_solver import export_weights_header

        export_weights_header(net, args.export_header, board_size=args.board_size)
        print(f"Saved C header policy: {args.export_header}")

    print(f"Training time: {time.time() - t0:.2f}s")
    return 0


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="EvoTorch PAD solver/training entrypoint"
    )
    sub = parser.add_subparsers(dest="command")

    solve_p = sub.add_parser("solve", help="Solve a single board")
    solve_p.add_argument(
        "board",
        nargs="?",
        default="LHDDGLRDHHRHGGLGRGRDDRBLHLBHGL",
        help="Board string to solve",
    )
    solve_p.add_argument("--max-steps", type=int, default=55)
    solve_p.add_argument("--min-erase", type=int, default=3)
    solve_p.add_argument("--popsize", type=int, default=300)
    solve_p.add_argument("--generations", type=int, default=1000)
    solve_p.add_argument("--restarts", type=int, default=5)
    solve_p.add_argument("--stdev-init", type=float, default=2.5)
    solve_p.add_argument("--refine-iters", type=int, default=8000)
    solve_p.add_argument(
        "--beam-size",
        type=int,
        default=320,
        help="Lookahead beam width used by planner/hybrid solve",
    )
    solve_p.add_argument("--quiet", action="store_true")
    solve_p.set_defaults(func=_cmd_solve)

    train_p = sub.add_parser("train", help="Train a generalized policy")
    train_p.add_argument("--board-size", type=int, default=30, choices=[20, 30, 42])
    train_p.add_argument("--num-boards", type=int, default=50)
    train_p.add_argument("--max-steps", type=int, default=55)
    train_p.add_argument("--min-erase", type=int, default=3)
    train_p.add_argument("--num-starts", type=int, default=6)
    train_p.add_argument("--hidden", type=int, default=64)
    train_p.add_argument(
        "--lookahead-steps",
        type=int,
        default=10,
        help="Moves planned ahead per network query (default 10)",
    )
    train_p.add_argument("--popsize", type=int, default=100)
    train_p.add_argument("--generations", type=int, default=200)
    train_p.add_argument(
        "--boards-per-generation",
        type=int,
        default=0,
        help=(
            "Streaming training: generate this many fresh boards per fitness "
            "evaluation call (0 = use --num-boards fixed boards). "
            "Use 64+ to train across millions of unique boards."
        ),
    )
    train_p.add_argument("--eval-board", type=str, default="")
    train_p.add_argument("--export-torchscript", type=str, default="")
    train_p.add_argument("--export-header", type=str, default="")
    train_p.add_argument("--quiet", action="store_true")
    train_p.set_defaults(func=_cmd_train)

    return parser


def main() -> int:
    parser = _build_parser()
    args = parser.parse_args()

    if not getattr(args, "command", None):
        # Backward-compatible default: run solve with defaults
        args = parser.parse_args(["solve"])

    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
