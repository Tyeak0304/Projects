import csv
import subprocess
from conftest import BINARY, RESULTS_CSV, PROJECT_ROOT, TESTING_CSV

# Node IDs that are marked as hardened (protected by a firewall) in the test data.
# These nodes should never show up as infected in the results.
HARDENED_IDS = {1015, 1022, 1038, 1047, 1056, 1063, 1071, 1084, 1092, 1099}

# Node IDs that start the simulation already infected (the "patient zero" nodes).
# These are the origin points of the outbreak and must always appear in the results.
PATIENT_ZERO_IDS = {1001, 1002, 1003}


# =============================================================================
# SETUP — make sure the program was compiled before running any tests
# =============================================================================

def test_binary_exists():
    # Checks that the compiled Game binary actually exists on disk.
    # If this fails, run 'make' in the project root first.
    assert BINARY.exists(), f"Binary not found at {BINARY} — run 'make' first."


# =============================================================================
# BASIC EXECUTION — does the program run correctly and handle bad input?
# =============================================================================

def test_basic_run_exits_zero(run_game):
    # A normal run with valid input files should finish without any errors.
    # Exit code 0 means the program completed successfully.
    result = run_game()
    assert result.returncode == 0, f"stderr: {result.stderr}"


def test_no_args_exits_nonzero():
    # If the user runs the program without providing any CSV files,
    # it should fail (exit code anything other than 0) instead of crashing silently.
    result = subprocess.run([str(BINARY)], capture_output=True, text=True, cwd=PROJECT_ROOT)
    assert result.returncode != 0


def test_no_args_prints_usage():
    # When run with no arguments, the program should print instructions
    # telling the user how to use it correctly.
    result = subprocess.run([str(BINARY)], capture_output=True, text=True, cwd=PROJECT_ROOT)
    assert "Usage" in result.stderr


def test_missing_file_exits_nonzero(run_game):
    # If a file that doesn't exist is passed as input, the program should
    # fail gracefully rather than crash with an unhandled error.
    result = subprocess.run(
        [str(BINARY), "nonexistent.csv",
         str(TESTING_CSV / "test_hardened_nodes.csv"),
         str(TESTING_CSV / "test_edges.csv"),
         "--no-animate"],
        capture_output=True, text=True, cwd=PROJECT_ROOT,
    )
    assert result.returncode != 0


# =============================================================================
# OUTPUT FILE — does the program produce the correct results CSV?
# =============================================================================

def test_output_csv_created(run_game):
    # After a successful run, the results file should exist in the Results/ folder.
    run_game()
    assert RESULTS_CSV.exists()


def test_output_csv_headers(run_game):
    # The first row of the CSV should be exactly these column names, in this order.
    # If the headers are wrong, anything reading the CSV (scripts, spreadsheets) will break.
    run_game()
    with open(RESULTS_CSV) as f:
        reader = csv.DictReader(f)
        assert reader.fieldnames == ["node_id", "accepted_payload", "infection_path"]


def test_output_csv_has_rows(run_game):
    # The simulation should actually infect at least one node.
    # An empty results file means the infection never spread, which is a bug.
    run_game()
    with open(RESULTS_CSV) as f:
        rows = list(csv.DictReader(f))
    assert len(rows) > 0


# =============================================================================
# INFECTION LOGIC — did the simulation behave correctly?
# =============================================================================

def test_patient_zero_nodes_are_infected(run_game):
    # The nodes that start as infected (patient zero) must always appear in the output.
    # If they are missing, the simulation didn't record its own starting points.
    run_game()
    with open(RESULTS_CSV) as f:
        infected_ids = {int(row["node_id"]) for row in csv.DictReader(f)}
    for node_id in PATIENT_ZERO_IDS:
        assert node_id in infected_ids, f"Patient zero node {node_id} missing from results"


def test_hardened_nodes_not_infected(run_game):
    # Hardened nodes have firewalls and should block the infection entirely.
    # If any hardened node appears in the results, the firewall logic has a bug.
    run_game()
    with open(RESULTS_CSV) as f:
        infected_ids = {int(row["node_id"]) for row in csv.DictReader(f)}
    for node_id in HARDENED_IDS:
        assert node_id not in infected_ids, f"Hardened node {node_id} should be blocked"


def test_infection_path_starts_at_patient_zero(run_game):
    # Every infected node's path should trace back to one of the patient zero nodes.
    # The path is stored as a colon-separated list of node IDs (e.g. "1001:1006:1026").
    # The first ID in that list should always be a patient zero node.
    run_game()
    with open(RESULTS_CSV) as f:
        for row in csv.DictReader(f):
            path = [int(x) for x in row["infection_path"].split(":")]
            assert path[0] in PATIENT_ZERO_IDS, f"Path {path} doesn't start at a patient zero node"


def test_infection_path_format(run_game):
    # The infection path column should only contain numbers separated by colons.
    # Anything else (letters, spaces, extra punctuation) means the path was recorded wrong.
    run_game()
    with open(RESULTS_CSV) as f:
        for row in csv.DictReader(f):
            parts = row["infection_path"].split(":")
            assert all(p.isdigit() for p in parts), f"Bad infection path format: {row['infection_path']}"


def test_payload_names_are_non_empty(run_game):
    # Every infected node should have the name of the malware that infected it.
    # A blank payload name means the infection was recorded without knowing what caused it.
    run_game()
    with open(RESULTS_CSV) as f:
        for row in csv.DictReader(f):
            assert row["accepted_payload"].strip() != "", f"Node {row['node_id']} has empty payload"


# =============================================================================
# FLAGS — do the optional command-line flags work without breaking anything?
# =============================================================================

def test_verbose_flag_exits_zero(run_game):
    # Running with --verbose should still complete successfully.
    # This flag enables detailed step-by-step output but shouldn't cause any errors.
    result = run_game("--verbose")
    assert result.returncode == 0, f"stderr: {result.stderr}"


def test_verbose_flag_produces_output(run_game):
    # With --verbose, the program should actually print something to the terminal.
    # If stdout is empty, the verbose mode isn't working.
    result = run_game("--verbose")
    assert len(result.stdout) > 0
