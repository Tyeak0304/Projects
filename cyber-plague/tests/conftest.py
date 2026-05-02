import subprocess
import os
from pathlib import Path
import pytest

PROJECT_ROOT = Path(__file__).parent.parent
BINARY = PROJECT_ROOT / ("Game.exe" if os.name == "nt" else "Game")
TESTING_CSV = PROJECT_ROOT / "testing_csv"
RESULTS_CSV = PROJECT_ROOT / "Results" / "infection_results.csv"


@pytest.fixture(autouse=True)
def clean_results():
    if RESULTS_CSV.exists():
        RESULTS_CSV.unlink()
    yield


@pytest.fixture
def run_game():
    def _run(*extra_args):
        return subprocess.run(
            [
                str(BINARY),
                str(TESTING_CSV / "test_patient_zero.csv"),
                str(TESTING_CSV / "test_hardened_nodes.csv"),
                str(TESTING_CSV / "test_edges.csv"),
                "--no-animate",
                *extra_args,
            ],
            capture_output=True,
            text=True,
            cwd=PROJECT_ROOT,
        )
    return _run
