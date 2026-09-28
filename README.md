# Home Assistant Development

Development workspace for a vibration-based dryer activity sensor that reports START and STOP events to Home Assistant.

## Project direction

The final product is an ESP32 running C++ firmware. Python is a PC-side development and verification tool for waveform analysis, hardware mocks, replaying recorded measurements, and testing the detection algorithm.

An M5Stack with an MPU-6050 is the temporary device for early development and full waveform collection. The intended final ESP32 is low-power: Wi-Fi stays off normally and turns on only after vibration analysis detects START or STOP, so the event can be sent to Home Assistant. Once the dedicated ESP32 is available, operate it alongside the M5Stack and compare its decisions with the measured waveforms.

Keep hardware acquisition separate from detection logic so that the logic can be tested on a PC with mocks and recorded waveforms. Sampling details, thresholds, power targets, communication protocol, and the exact Home Assistant integration are not yet specified.

## Repository layout

```text
.
├── data/                         # Local development datasets
├── dev/Dockerfile                # Python development container image
├── docker-compose.yml            # Python development service
├── docs/design.md                # Placeholder for an agreed design specification
└── src/dryer-sensor/
    ├── firmware/                 # ESP32 C++ firmware (src/, test/)
    └── python/                   # Analysis and validation (src/dryer_sensor/, tests/)
```

The source and test directories are planned and may not contain implementation yet. Firmware uses `test/` for the intended PlatformIO convention; Python uses `tests/`.

## Development container

The development container provides Python for waveform analysis and validation. The repository is mounted at `/workspace`, and local datasets are mounted at `/data`.

Run the setup script once from the repository root to write a local `.env` with your Ubuntu UID, primary GID, and account name. The `.env` file is ignored by Git. The image creates that account, installs `sudo`, and runs as that user.

```sh
./setup.sh
docker compose up --build -d
```

Generate a CSV and PNG from the synthetic idle/running vibration traces with:

```sh
docker compose exec python-dev python -m dryer_sensor.export_mock_vibration
```

By default, outputs go to `data/processed/mock_vibration/`. The script accepts `--output-dir`, `--sample-count`, and `--seed`. It writes one CSV containing both traces, then reads that CSV to create the plot. Open a shell with `docker compose exec python-dev sh`. Run Python tests with `docker compose exec python-dev pytest -q src/dryer-sensor/python/tests`. Stop the container with `docker compose down`. Development dependencies listed in `src/dryer-sensor/python/requirements-dev.txt` are installed in the image.

## Design documentation

`docs/design.md` exists as a placeholder; no detailed design has been agreed. Use it later to record stable decisions such as system boundaries, detection behavior, data formats, power strategy, and the Home Assistant communication flow. For now, this README and `AGENTS.md` capture project context without presenting unsettled choices as decisions. Keep the future design document in `docs/design.md`; a separate one under `src/dryer-sensor/` is unnecessary unless that subproject needs its own detailed specification.

## Data

Raw and processed data under `data/` are excluded from Git by `.gitignore`. Keep local captures there. Review size and privacy before adding small fixtures needed for reproducible tests.
