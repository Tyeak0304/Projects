#pragma once
#include <string>
#include <vector>
#include "simulation/Simulator.h"

// Saves the simulation results to a CSV file so they can be
// opened in a spreadsheet, read by another script, or reviewed later.
class OutputWriter {
public:
    // Write one row per infected node with three columns:
    //   node_id        — the machine's ID number
    //   accepted_payload — which malware infected it
    //   infection_path — the route the malware took, written as IDs separated by colons (e.g. "1001:1006:1026")
    void writeResults(const std::vector<InfectionResult>& results, const std::string& filename);
};
