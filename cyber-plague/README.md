# Cyber-Plague: Lateral Movement Simulator
# Agentic C++ Coding Project
**Course:** CSE3150 | **Language:** C++17

## Overview
Simulates malware propagation across a 100,000+ node enterprise network using
hierarchical lateral movement (Up/Across/Down) and policy-based conflict resolution.

## Project Structure
```
cyber-plague/
├── include/              # Header files
│   ├── nodes/            # Node base class & NodeType/InfectionState enums
│   ├── policies/         # Policy base, StandardPolicy, HardenedPolicy (ROV equiv.)
│   ├── network/          # Graph + DFS cycle detection
│   ├── simulation/       # Propagation engine (BFS)
│   └── io/               # CSV input parser & output writer
├── src/                  # .cpp implementations (mirrors include/)
├── tests/                # Unit tests
├── data/
│   ├── input/            # patient_zero.csv, hardened_nodes.csv
│   └── output/           # infection_results.csv (generated at runtime)
├── docs/
└── CMakeLists.txt
```

## Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j2
```

## Run
```bash
./cyber-plague ../data/input/patient_zero.csv ../data/input/hardened_nodes.csv
```

## Design Notes
| Concept (BGP) | Analogue (Cyber-Plague) |
|---|---|
| Autonomous System | Network Node (workstation/server/IoT) |
| Route announcement | Malware payload |
| Path selection | Policy conflict resolution (cred level → efficiency) |
| ROV | HardenedPolicy (IDS/Firewall drops payloads) |
| Provider/Peer/Customer | Up (servers) / Across (workstations) / Down (IoT) |

## Propagation Order
Up → Across → Down (per Propagation Rank logic)

## Memory Notes
`std::unordered_map<uint32_t, Node>` gives O(1) lookup across 100k+ nodes.
Custom data structures avoid heap fragmentation at scale.
