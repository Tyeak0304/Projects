#pragma once
#include <string>
#include <vector>
#include "simulation/Simulator.h"

class OutputWriter {
public:
    // Writes infection_results.csv: node_id, payload, infection_path
    void writeResults(const std::vector<InfectionResult>& results,
                      const std::string& outputPath);
};
