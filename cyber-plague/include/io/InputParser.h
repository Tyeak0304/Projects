#pragma once
#include <string>
#include <memory>
#include "network/Network.h"

// Reads the three input CSV files and assembles a Network ready for simulation.
class InputParser {
public:
    // Read patient_zero.csv and mark those nodes as already infected
    // with their starting malware payload.
    void readPatientZero(const std::string& filename, Network& network);

    // Read hardened_nodes.csv and upgrade those nodes to use a HardenedPolicy,
    // making them immune to infection unless a very powerful payload arrives.
    void readHardenedNodes(const std::string& filename, Network& network);

    // Read edges.csv and wire up the connections between nodes.
    // Any node mentioned in the edges file that doesn't already exist
    // is automatically created as a clean, unprotected workstation.
    void readEdges(const std::string& filename, Network& network);

    // Reads all three files and returns a fully built, ready-to-run Network.
    // This is the main entry point — call this instead of the three functions above.
    std::unique_ptr<Network> buildNetwork(const std::string& patientZeroFile,
                                          const std::string& hardenedNodesFile,
                                          const std::string& edgesFile);
};
