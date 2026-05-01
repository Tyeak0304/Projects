# Cyber-Plague: Lateral Movement Simulator
**Course:** CSE3150 | **Language:** C++17

## Overview
Simulates malware propagation across a network using hierarchical lateral movement (Up/Across/Down) and policy-based infection resolution.

## Project Structure
```
cyber-plague/
├── include/              # Header files
│   ├── nodes/            # Node base class & NodeType/InfectionState enums
│   ├── policies/         # Policy base, StandardPolicy, HardenedPolicy
│   ├── network/          # Graph + DFS cycle detection
│   ├── simulation/       # Propagation engine (BFS)
│   └── io/               # CSV input parser & output writer
├── src/                  # .cpp implementations (mirrors include/)
├── data/
│   ├── input/            # patient_zero.csv, hardened_nodes.csv, edges.csv
│   └── output/           # infection_results.csv (generated at runtime)
└── CMakeLists.txt
```

## Dependencies
- CMake 3.16+
- A C++17 compiler (GCC, Clang, or MSVC)
- On Windows: [MSYS2/MinGW-w64](https://www.msys2.org/) recommended

## Compiling

**Windows (MSYS2/MinGW)**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Ninja"
cmake --build .
```

**Linux / macOS**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

> **Note:** The Debug build uses `-fsanitize=address,undefined` which requires
> `libasan`/`libubsan`. If your toolchain does not include them, use
> `-DCMAKE_BUILD_TYPE=Release` as shown above.

## Running
```bash
./build/cyber-game <patient_zero.csv> <hardened_nodes.csv> <edges.csv> [--verbose|-v] [--no-animate]
```

| Flag | Effect |
|---|---|
| *(none)* | Animated dot-map mode (default) |
| `--verbose` / `-v` | Step-by-step text output |
| `--no-animate` | Plain output, no animation |

**Example**
```bash
./build/cyber-game data/input/patient_zero.csv data/input/hardened_nodes.csv data/input/edges.csv
```

## Input File Format

**patient_zero.csv** — starting infected nodes
```
nodeId,payloadName,exploitEfficiency,credentialLevel
1001,ransomware_v1,90,3
```

**hardened_nodes.csv** — immune nodes
```
nodeId,firewallLevel
1015,3
```

**edges.csv** — network topology (auto-creates unknown nodes as clean workstations)
```
from_id,to_id,edge_type
1001,2001,customer
2001,3001,peer
```
Edge types: `provider`, `peer`, `customer`

## Design Notes
| Concept (BGP) | Analogue (Cyber-Plague) |
|---|---|
| Autonomous System | Network Node (workstation/server/IoT) |
| Route announcement | Malware payload |
| ROV | HardenedPolicy (IDS/Firewall blocks infection) |
| Provider/Peer/Customer | Up (servers) / Across (workstations) / Down (IoT) |

## Propagation Order
Up → Across → Down (providers first, then peers, then customers)
