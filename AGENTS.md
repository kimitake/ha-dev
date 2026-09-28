# Instructions for Codex

## Project purpose

This repository develops a vibration-based dryer activity sensor for Home Assistant. The final device is an ESP32 running C++ firmware. Python is for waveform analysis, mocks, replaying recorded data, and validating detection algorithms on a PC; it is not the final product runtime.

## Current goals and assumptions

- Use an M5Stack with an MPU-6050 as the temporary development and measurement device. Collect full vibration waveforms during early development.
- The final low-power ESP32 detects dryer START and STOP events from vibration. Keep Wi-Fi off during normal operation; enable it only to send a detected event to Home Assistant, then return to low-power operation.
- Once the dedicated ESP32 is available, operate it alongside the M5Stack and compare its decisions with the M5Stack's measured waveforms.
- Separate sensor/device acquisition from detection logic. Keep detection logic testable on a PC with mocks and recorded waveforms.
- These are goals, not a finalized specification. Do not invent sampling parameters, thresholds, power budgets, network protocols, or integration details.

## Repository guidance

- `src/dryer-sensor/firmware/` is the C++ embedded subproject; `src/` and `test/` follow the intended PlatformIO-style layout.
- `src/dryer-sensor/python/` is for analysis and validation; package code belongs in `src/dryer_sensor/` and tests in `tests/`.
- Keep tests with their subproject. Use hyphens for project directories and underscores for Python package names.
- `data/` holds local datasets. Raw and processed data are ignored by Git; do not add large captures without an explicit request.
- `dev/Dockerfile` and root `docker-compose.yml` are planned for a reproducible development container. The next container work should first support the Python workflow. Inspect current files and document commands only once implemented.
- `docs/design.md` is a placeholder for an agreed design specification. Do not treat it as authoritative or fill in unsettled decisions. Add design content when requirements and decisions are concrete.
- Read existing files before editing, preserve existing work, and distinguish planned features from implemented ones.
