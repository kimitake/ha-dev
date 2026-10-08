"""Deterministic synthetic vibration traces for PC-side development.

Samples are in arbitrary units and are not calibrated to any sensor or dryer.
"""

from dataclasses import dataclass
import math
import random
from typing import Literal


Activity = Literal["idle", "running"]


@dataclass(frozen=True)
class MockVibration:
    """A generated trace and its simulated activity label."""

    activity: Activity
    samples: tuple[float, ...]


def generate_mock_vibration(
    activity: Activity,
    *,
    sample_count: int = 256,
    seed: int = 0,
) -> MockVibration:
    """Generate a repeatable idle or running trace in arbitrary units.

    The periodic running signal is intentionally a simple synthetic pattern,
    not a model of a particular dryer, sensor, or sampling rate.
    """
    if activity not in ("idle", "running"):
        raise ValueError("activity must be 'idle' or 'running'")
    if sample_count <= 0:
        raise ValueError("sample_count must be positive")

    rng = random.Random(seed)
    samples = []
    for index in range(sample_count):
        noise = rng.gauss(0.0, 0.03)
        if activity == "running":
            phase = 2.0 * math.pi * index / 32.0
            vibration = 0.6 * math.sin(phase) + 0.15 * math.sin(3.0 * phase)
        else:
            vibration = 0.0
        samples.append(vibration + noise)

    return MockVibration(activity=activity, samples=tuple(samples))
