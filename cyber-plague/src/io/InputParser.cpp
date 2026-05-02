#include "io/InputParser.h"
#include "nodes/Node.h"
#include "policies/StandardPolicy.h"
#include "policies/HardenedPolicy.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// Read patient_zero.csv and add each listed node to the network as already infected.
// Each row gives the node's ID, the malware name, and some payload stats.
void InputParser::readPatientZero(const std::string& filename, Network& network) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open patient zero file: " + filename);
    }
    std::string line;
    std::getline(file, line); // skip the header row
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t nodeId;
        std::string payloadName;
        int exploitEfficiency;
        int credentialLevel;
        std::getline(ss, cell, ','); nodeId = std::stoul(cell);
        std::getline(ss, cell, ','); payloadName = cell;
        std::getline(ss, cell, ','); exploitEfficiency = std::stoi(cell);
        std::getline(ss, cell, ','); credentialLevel = std::stoi(cell);

        // Create the node, mark it as infected from the start, and record which payload it carries.
        auto node = std::make_unique<Node>(nodeId, NodeType::WORKSTATION);
        node->policy = std::make_unique<StandardPolicy>();
        node->state = InfectionState::INFECTED;
        node->acceptedPayload = payloadName;
        node->infectionPath = { nodeId }; // the infection path starts at this node itself
        network.addNode(std::move(node));
    }
}

// Read hardened_nodes.csv and upgrade each listed node to use a HardenedPolicy.
// The node must already exist in the network (added via readPatientZero or readEdges).
// If a matching node is found, its policy is replaced and its state is set to HARDENED.
void InputParser::readHardenedNodes(const std::string& filename, Network& network) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open hardened nodes file: " + filename);
    }
    std::string line;
    std::getline(file, line); // skip the header row
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t nodeId;
        int firewallLevel;
        std::getline(ss, cell, ','); nodeId = std::stoul(cell);
        std::getline(ss, cell, ','); firewallLevel = std::stoi(cell);

        // Look up the node and swap in a HardenedPolicy with the specified firewall strength.
        Node* node = network.getNode(nodeId);
        if (node) {
            node->policy = std::make_unique<HardenedPolicy>(firewallLevel);
            node->state = InfectionState::HARDENED;
        }
    }
}

// Read edges.csv and create a connection between every pair of nodes listed.
// If a node ID appears in the edges file but hasn't been created yet,
// it is automatically added as a clean, unprotected workstation.
void InputParser::readEdges(const std::string& filename, Network& network) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open edges file: " + filename);
    }
    std::string line;
    std::getline(file, line); // skip the header row
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t fromId, toId;
        std::string edgeType;
        std::getline(ss, cell, ','); fromId = std::stoul(cell);
        std::getline(ss, cell, ','); toId = std::stoul(cell);
        std::getline(ss, edgeType); // read the rest of the line as the edge type

        // Auto-create any node that appears in the edges file but doesn't exist yet.
        if (!network.getNode(fromId)) {
            auto node = std::make_unique<Node>(fromId, NodeType::WORKSTATION);
            node->policy = std::make_unique<StandardPolicy>();
            network.addNode(std::move(node));
        }
        if (!network.getNode(toId)) {
            auto node = std::make_unique<Node>(toId, NodeType::WORKSTATION);
            node->policy = std::make_unique<StandardPolicy>();
            network.addNode(std::move(node));
        }

        network.addEdge(fromId, toId, edgeType);
    }
}

// Main entry point: read all three CSV files in the correct order and return a finished Network.
// Order matters — edges must be read before hardened nodes so the nodes already exist
// when we try to assign a HardenedPolicy to them.
std::unique_ptr<Network> InputParser::buildNetwork(const std::string& patientZeroPath,
                                                    const std::string& hardenedPath,
                                                    const std::string& edgesPath) {
    auto network = std::make_unique<Network>();
    readPatientZero(patientZeroPath, *network);
    readEdges(edgesPath, *network);
    readHardenedNodes(hardenedPath, *network);
    return network;
}
