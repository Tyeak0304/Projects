#pragma once
#include <string>
#include <vector>
#include "simulation/Simulator.h"

// TODO: Define OutputWriter class
// TODO: Add method to write infection_results.csv (node_id, payload, infection_path)

class OutputWriter{
    public:
        void writeInfectionResults(const std::vector<InfectionResult>& results, const std::string& filename);
};