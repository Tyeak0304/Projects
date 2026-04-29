#pragma once
#include <string>
#include <memory>
#include "network/Network.h"

// TODO: Define InputParser class
// TODO: Add method to read patient_zero.csv and hardened_nodes.csv
// TODO: Return a fully-built Network from buildNetwork()

class InputParser {
    public: 
        void readPatientZero(const std::string& filename, Network& network);
    
};
