#pragma once
#include <string>
#include <memory>
#include "network/Network.h"

class InputParser {
public:
    // Reads patient_zero.csv and hardened_nodes.csv, returns a fully-built Network
    std::unique_ptr<Network> buildNetwork(const std::string& patientZeroPath,
                                          const std::string& hardenedPath);
};
