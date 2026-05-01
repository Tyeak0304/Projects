#pragma once
#include <string>
#include <memory>
#include "network/Network.h"

class InputParser {
    public:
        // Reads patient_zero.csv and marks matching nodes as INFECTED with their starting payload
        void readPatientZero(const std::string& filename, Network& network);

        // Reads hardened_nodes.csv and assigns a HardenedPolicy to each listed node
        void readHardenedNodes(const std::string& filename, Network& network);

        // Parses both CSVs and returns a fully constructed, ready-to-simulate Network
        std::unique_ptr<Network> buildNetwork(const std::string& patientZeroFile, const std::string& hardenedNodesFile);
};
