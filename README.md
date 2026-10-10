# c-offshore-redundant-sensor-voter

Detect disagreement and stale measurements across three pressure channels using median voting and hysteresis.

```sh
make test
./app < examples/input.txt
```

Offline diagnostic tool. Inputs and limits must be validated for each installation; no equipment commands are sent.

MIT. Built by [brunnodev](https://brunnodev.store).

## Execution proof

[![Executable proof](https://github.com/brunnojob/c-offshore-redundant-sensor-voter/actions/workflows/proof.yml/badge.svg)](https://github.com/brunnojob/c-offshore-redundant-sensor-voter/actions/workflows/proof.yml)

![Recorded program execution](docs/proof/execution.svg)

[Verified run](https://github.com/brunnojob/c-offshore-redundant-sensor-voter/actions/runs/38018099989) · [Execution report](docs/proof/evidence.json)

Run `python .proof/record.py` after installing the prerequisites above. The scenarios execute repository code and verify exit codes and expected output. CI publishes `execution-proof` with the transcript, input fingerprints and source commit. The downloadable report identifies the exact tested version; the workflow badge tracks the latest run.
