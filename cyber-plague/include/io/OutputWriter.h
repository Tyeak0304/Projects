#pragma once
#include <string>
#include <vector>
#include "simulation/Simulator.h"

class OutputWriter{
    public:
        // Writes one CSV row per infected node: node_id, accepted_payload, colon-separated infection path
        void writeResults(const std::vector<InfectionResult>& results, const std::string& filename);
};
