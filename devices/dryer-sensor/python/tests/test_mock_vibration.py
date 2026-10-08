import math

import pytest

from dryer_sensor.mock_vibration import generate_mock_vibration


def rms(samples: tuple[float, ...]) -> float:
    return math.sqrt(sum(sample * sample for sample in samples) / len(samples))


def test_generates_requested_number_of_finite_samples():
    trace = generate_mock_vibration("running", sample_count=128)

    assert trace.activity == "running"
    assert len(trace.samples) == 128
    assert all(math.isfinite(sample) for sample in trace.samples)


def test_same_seed_reproduces_the_same_trace():
    first = generate_mock_vibration("running", sample_count=64, seed=42)
    second = generate_mock_vibration("running", sample_count=64, seed=42)

    assert first == second


def test_running_mock_has_more_energy_than_idle_mock():
    idle = generate_mock_vibration("idle", seed=17)
    running = generate_mock_vibration("running", seed=17)

    assert rms(running.samples) > rms(idle.samples)


def test_rejects_unknown_activity_and_nonpositive_sample_count():
    with pytest.raises(ValueError, match="activity"):
        generate_mock_vibration("paused")  # type: ignore[arg-type]
    with pytest.raises(ValueError, match="sample_count"):
        generate_mock_vibration("idle", sample_count=0)
