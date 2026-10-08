"""Export synthetic idle/running vibration traces to CSV and a PNG plot."""

import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

from dryer_sensor.mock_vibration import Activity, generate_mock_vibration


DEFAULT_OUTPUT_DIR = Path("data/processed/mock_vibration")


def export_mock_vibration(
    output_dir: Path,
    *,
    sample_count: int = 256,
    seed: int = 0,
) -> tuple[Path, Path]:
    """Write both activity traces to one CSV and plot that CSV to a PNG."""
    output_dir.mkdir(parents=True, exist_ok=True)
    csv_path = output_dir / "mock_vibration.csv"
    png_path = output_dir / "mock_vibration.png"

    traces = [
        generate_mock_vibration(activity, sample_count=sample_count, seed=seed)
        for activity in ("idle", "running")
    ]

    with csv_path.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(("activity", "sample_index", "value_au"))
        for trace in traces:
            for index, value in enumerate(trace.samples):
                writer.writerow((trace.activity, index, f"{value:.8f}"))

    # Read the saved CSV back so the visualization demonstrates the same
    # data-loading path that can be used with other recorded CSV files.
    series: dict[Activity, tuple[list[int], list[float]]] = {
        "idle": ([], []),
        "running": ([], []),
    }
    with csv_path.open(newline="", encoding="utf-8") as csv_file:
        for row in csv.DictReader(csv_file):
            activity = row["activity"]
            indices, values = series[activity]  # type: ignore[index]
            indices.append(int(row["sample_index"]))
            values.append(float(row["value_au"]))

    figure, axes = plt.subplots(2, 1, figsize=(10, 6), sharex=True)
    for axis, activity in zip(axes, ("idle", "running")):
        indices, values = series[activity]
        axis.plot(indices, values, linewidth=0.9)
        axis.set_title(f"{activity.capitalize()} (synthetic)")
        axis.set_ylabel("Vibration (arbitrary units)")
        axis.grid(True, alpha=0.3)
    axes[-1].set_xlabel("Sample index")
    figure.suptitle("Mock dryer vibration traces")
    figure.tight_layout()
    figure.savefig(png_path, dpi=150)
    plt.close(figure)

    return csv_path, png_path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help=f"directory for CSV and PNG outputs (default: {DEFAULT_OUTPUT_DIR})",
    )
    parser.add_argument(
        "--sample-count",
        type=int,
        default=256,
        help="number of samples per activity trace",
    )
    parser.add_argument("--seed", type=int, default=0, help="random seed")
    args = parser.parse_args()

    csv_path, png_path = export_mock_vibration(
        args.output_dir,
        sample_count=args.sample_count,
        seed=args.seed,
    )
    print(f"CSV: {csv_path}")
    print(f"PNG: {png_path}")


if __name__ == "__main__":
    main()
