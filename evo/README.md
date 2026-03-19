# Evo Solver Notes

This folder contains the EvoTorch implementation and tests.

## Generalization Runner

Use `generalization_runner.py` to train a policy on a random training split and
evaluate on unseen boards from a validation split.

### Quick run

```shell
uv run .\generalization_runner.py --generations 60 --train-boards 20 --valid-boards 30
```

### Save report JSON

```shell
uv run .\generalization_runner.py --output .\reports\generalization.json
```

Main metrics printed:

- `goal_rate`: fraction of validation boards where combo reached max_combo.
- `mean_combo_ratio`: mean of `combo / max_combo` across validation boards.
- `median_combo_ratio`: median of `combo / max_combo`.
