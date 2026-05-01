#include "io/InputParser.h"
#include "nodes/Node.h"
#include "policies/StandardPolicy.h"
#include "policies/HardenedPolicy.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// Parses hardened_nodes.csv first to build a privilege map, then parses patient_zero.csv
// to create nodes, assign policies, and mark each as an initial infection source.
std::unique_ptr<Network> InputParser::buildNetwork(const std::string& patientZeroPath,
                                                    const std::string& hardenedPath) {
    auto network = std::make_unique<Network>();

    // First pass: load hardened node IDs and their firewall levels into a lookup map
    std::unordered_map<uint32_t, int> hardenedMap;
    std::ifstream hardenedFile(hardenedPath);
    if (!hardenedFile.is_open()) {
        throw std::runtime_error("Failed to open hardened nodes file: " + hardenedPath);
    }
    std::string line;
    std::getline(hardenedFile, line); // skip CSV header row
    while (std::getline(hardenedFile, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t nodeId;
        int firewallLevel;
        std::getline(ss, cell, ',');
        nodeId = std::stoul(cell);
        std::getline(ss, cell, ',');
        firewallLevel = std::stoi(cell);
        hardenedMap[nodeId] = firewallLevel;
    }
    hardenedFile.close();

    // Second pass: create each Patient Zero node and assign the appropriate policy
    std::ifstream patientZeroPathFile(patientZeroPath);
    if (!patientZeroPathFile.is_open()) {
        throw std::runtime_error("Failed to open patient zero file: " + patientZeroPath);
    }
    std::getline(patientZeroPathFile, line); // skip CSV header row
    while (std::getline(patientZeroPathFile, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t nodeId;
        std::string payloadName;
        int exploitEfficiency;
        int credentialLevel;
        std::getline(ss, cell, ',');
        nodeId = std::stoul(cell);
        std::getline(ss, cell, ',');
        payloadName = cell;
        std::getline(ss, cell, ',');
        exploitEfficiency = std::stoi(cell);
        std::getline(ss, cell, ',');
        credentialLevel = std::stoi(cell);

        // All patient zero nodes are treated as workstations in this simplified model
        auto node = std::make_unique<Node>(nodeId, NodeType::WORKSTATION);

        // Nodes in hardenedMap get a threshold-based filter; all others accept all payloads
        if (hardenedMap.count(nodeId)){
            node->policy = std::make_unique<HardenedPolicy>(hardenedMap[nodeId]);
        } else {
            node->policy = std::make_unique<StandardPolicy>();
        }

        node->state = InfectionState::INFECTED;
        node->acceptedPayload = payloadName;
        node->infectionPath = { nodeId }; // infection path starts with the node itself
        network->addNode(std::move(node));
    }
    patientZeroPathFile.close();

    return network;
}
