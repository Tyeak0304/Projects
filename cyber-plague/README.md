# Cyber-Plague: Lateral Movement Simulator

**Course:** CSE3150 | **Language:** C++17

---

## What Is This?

Cyber-Plague is a terminal-based simulation of how malware spreads across a computer network. You give it three CSV files — a list of already-infected machines, a list of protected machines, and a map of how all the machines are connected — and it simulates the outbreak wave by wave, showing you exactly which machines got infected, in what order, and through what path.

The network is modeled as a hierarchy — servers above, workstations in the middle, and IoT devices below. Malware spreads upward, sideways, and downward through that hierarchy, just like real lateral movement attacks.

---

## Features

- **Animated terminal display** — watch the infection spread across a color-coded map in real time, with each malware strain shown in a different color
- **Wave-by-wave BFS propagation** — infection spreads one hop at a time so you can see exactly how far it reaches each round
- **Policy-based infection control** — each machine has a policy that decides whether it accepts or blocks incoming malware
- **Hardened nodes** — machines with a firewall policy that blocks any malware below a minimum privilege threshold
- **CSV input and output** — load any network topology from files, and results are saved to `Results/infection_results.csv`
- **Multiple output modes** — animated map (default), verbose step-by-step text, or plain summary
- **Pytest test suite** — automated tests that run the binary and verify its behavior end to end

---

## Quick Start (Linux / macOS)

**Requirements:** `g++` (GCC 7+ or Clang 5+) and `make` — both come pre-installed on most Linux systems.
If you don't have them: `sudo apt install build-essential`

```bash
# 1. Build
make

# 2. Run with the included test data
./Game testing_csv/test_patient_zero.csv testing_csv/test_hardened_nodes.csv testing_csv/test_edges.csv

# 3. Find your results
cat Results/infection_results.csv
```

**Windows (CMake + MSYS2/MinGW)**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Ninja"
cmake --build .
.\Game.exe testing_csv\test_patient_zero.csv testing_csv\test_hardened_nodes.csv testing_csv\test_edges.csv
```

---

## Command-Line Flags

```
./Game <patient_zero.csv> <hardened_nodes.csv> <edges.csv> [flags]
```

| Flag | What it does |
|---|---|
| *(none)* | Animated color map — watch the infection spread live (default) |
| `--verbose` / `-v` | Prints every infection attempt one line at a time |
| `--no-animate` | Runs silently and just prints a final summary |

---

## Input File Format

The simulation takes three CSV files as input. Sample files are provided in `testing_csv/`.

**patient_zero.csv** — the machines that start out infected
```
nodeId,payloadName,exploitEfficiency,credentialLevel
1001,ransomware_v1,90,3
1002,spyware_x,75,2
```
- `exploitEfficiency` — how effective this malware is at breaking in (used as a tiebreaker)
- `credentialLevel` — the privilege level this malware carries (higher = harder to stop)

**hardened_nodes.csv** — machines with a firewall that blocks weak payloads
```
nodeId,firewallLevel
1015,3
1022,2
```
- `firewallLevel` — any payload with a `credentialLevel` below this number is blocked

**edges.csv** — the connections between machines
```
from_id,to_id,edge_type
1001,2001,customer
2001,3001,peer
```
- Edge types: `provider` (upstream), `peer` (same level), `customer` (downstream)
- Any machine that appears in this file but not in the other two is auto-created as a clean, unprotected workstation

---

## Project Structure

```
cyber-plague/
├── include/          # Header files — class definitions and data types
│   ├── nodes/        # Node class: represents one machine in the network
│   ├── policies/     # Policy base class, StandardPolicy, HardenedPolicy
│   ├── network/      # Network class: stores all machines and connections
│   ├── simulation/   # Simulator class: runs the BFS infection spread
│   └── io/           # InputParser (reads CSVs) and OutputWriter (writes results)
├── src/              # Implementation files — the actual logic behind each header
├── testing_csv/      # Sample input CSVs for running and testing the simulation
├── Results/          # infection_results.csv is written here after each run
├── tests/            # Automated pytest suite — see tests/README.md
├── Makefile          # Build with 'make', clean with 'make clean'
└── CMakeLists.txt    # Alternative build system for Windows/CMake users
```

---

## Design Decisions

### Why BFS (breadth-first search) for propagation?
BFS spreads infection one hop at a time, which naturally produces "waves." Every machine one hop from a patient zero gets infected before any machine two hops away. This mirrors how real lateral movement works — an attacker compromises one machine, then uses it to reach the next layer, and so on. It also makes the animation intuitive to watch.

### Why a Policy class instead of just an if-statement?
Every machine needs to decide whether to accept incoming malware. Instead of hard-coding that logic, each machine holds a `Policy` object that it delegates the decision to. This means adding a new kind of protection (e.g. a partially hardened node) only requires writing a new Policy subclass — none of the existing code needs to change. This pattern is called the Strategy pattern.

### Why provider → peer → customer spread order?
Malware that spreads upward first (to servers) is more dangerous because servers typically have access to everything below them. Spreading up before spreading sideways or down reflects real attacker behavior and means the simulation produces realistic worst-case results.

### Why CSV for input and output?
CSVs are the simplest format that is readable by both humans and programs. They can be opened directly in Excel or Google Sheets, processed by Python scripts, or diff'd in git. More complex formats (JSON, XML) would add overhead without adding value for this use case.

### Why does hardened node cycle detection exist?
If the network topology contained a circular connection (A connects to B connects to C connects back to A), the BFS wave loop would never terminate — it would keep re-visiting the same nodes forever. The cycle check runs once at startup and throws a clear error if a loop is found, rather than letting the simulation hang silently.

### Why colon-separated infection paths in the CSV?
Each infected node records the full chain of machines the malware traveled through to reach it (e.g. `1001:1006:1026:1061`). Storing this as a single colon-separated field keeps the CSV simple — one row per infected node — while still preserving the full audit trail without needing a separate table.

---

## Running Tests

```bash
make
pytest tests/
```

See [tests/README.md](tests/README.md) for a full breakdown of every test case.

---

## AI Assistance

This project used **Claude AI** as a development tool in several areas:

- **Debugging** — identifying logic errors and tracing unexpected behavior through the simulation
- **Code clarification** — clarified some concepts around C++ patterns and standard library functions
- **Code structure** — cleaning up inconsistent formatting and reorganizing messy sections for readability
- **Testing** — designing and implementing the pytest suite, including what edge cases to cover and how to structure fixtures
- **Animated terminal display** — implementing the color-coded animated map feature using ANSI escape codes, including the wave-by-wave redraw loop and the payload color palette