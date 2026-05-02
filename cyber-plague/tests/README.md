# Tests

The test suite uses **pytest** and works by running the compiled `Game` binary as a subprocess, then inspecting its exit code and output CSV.

## Requirements

- Python 3.7+
- pytest

```bash
pip install pytest
```

## Setup

Build the binary before running tests:

```bash
make
```

## Running

From the project root:

```bash
pytest tests/
```

Verbose output:

```bash
pytest tests/ -v
```

Stop on first failure:

```bash
pytest tests/ -x
```

## Test Files

| File | Purpose |
|---|---|
| `conftest.py` | Shared fixtures: binary path, result cleanup, `run_game` factory |
| `test_game.py` | All test cases |

## Test Cases

### Setup
| Test | What it checks |
|---|---|
| `test_binary_exists` | `Game` binary is present — fails immediately with a clear message if `make` was not run |

### Basic Execution
| Test | What it checks |
|---|---|
| `test_basic_run_exits_zero` | Normal run with test CSVs exits with code 0 |
| `test_no_args_exits_nonzero` | Running with no arguments exits non-zero |
| `test_no_args_prints_usage` | Usage message is printed to stderr when no arguments given |
| `test_missing_file_exits_nonzero` | Passing a nonexistent CSV exits non-zero |

### Output File
| Test | What it checks |
|---|---|
| `test_output_csv_created` | `Results/infection_results.csv` is created after a run |
| `test_output_csv_headers` | CSV header row is `node_id,accepted_payload,infection_path` |
| `test_output_csv_has_rows` | At least one infected node is written to the output |

### Infection Logic
| Test | What it checks |
|---|---|
| `test_patient_zero_nodes_are_infected` | Nodes 1001, 1002, 1003 appear in results |
| `test_hardened_nodes_not_infected` | Hardened nodes (1015, 1022, 1038, 1047, 1056, 1063, 1071, 1084, 1092, 1099) are absent from results |
| `test_infection_path_starts_at_patient_zero` | Every infection path begins at a patient zero node |
| `test_infection_path_format` | Infection paths are colon-separated integers (e.g. `1001:1006:1026`) |
| `test_payload_names_are_non_empty` | Every row has a non-empty payload name |

### Flags
| Test | What it checks |
|---|---|
| `test_verbose_flag_exits_zero` | `--verbose` flag runs without error |
| `test_verbose_flag_produces_output` | `--verbose` flag produces stdout output |

## Test Data

Tests use the CSVs in `testing_csv/`:

| File | Contents |
|---|---|
| `test_patient_zero.csv` | 3 infected starting nodes (ransomware_v1, spyware_x, rootkit_z) |
| `test_hardened_nodes.csv` | 10 hardened nodes with varying firewall levels |
| `test_edges.csv` | ~145 edges forming a multi-level customer/peer topology |

Results are written to `Results/infection_results.csv`. The `clean_results` fixture automatically removes this file before each test so runs don't interfere with each other.
