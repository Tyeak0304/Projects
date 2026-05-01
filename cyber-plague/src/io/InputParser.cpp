#include "io/InputParser.h"
#include "nodes/Node.h"
#include "policies/StandardPolicy.h"
#include "policies/HardenedPolicy.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

void InputParser::readPatientZero(const std::string& filename, Network& network) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open patient zero file: " + filename);
    }
    std::string line;
    std::getline(file, line); // skip header
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

        auto node = std::make_unique<Node>(nodeId, NodeType::WORKSTATION);
        node->policy = std::make_unique<StandardPolicy>();
        node->state = InfectionState::INFECTED;
        node->acceptedPayload = payloadName;
        node->infectionPath = { nodeId };
        network.addNode(std::move(node));
    }
}

// Reads hardened_nodes.csv and replaces the policy of any matching node already in the network
// with a HardenedPolicy using the specified firewall level.
void InputParser::readHardenedNodes(const std::string& filename, Network& network) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open hardened nodes file: " + filename);
    }
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t nodeId;
        int firewallLevel;
        std::getline(ss, cell, ','); nodeId = std::stoul(cell);
        std::getline(ss, cell, ','); firewallLevel = std::stoi(cell);

        Node* node = network.getNode(nodeId);
        if (node) {
            node->policy = std::make_unique<HardenedPolicy>(firewallLevel);
            node->state = InfectionState::HARDENED;
        }
    }
}

void InputParser::readEdges(const std::string& filename, Network& network) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open edges file: " + filename);
    }
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        uint32_t fromId, toId;
        std::string edgeType;
        std::getline(ss, cell, ','); fromId = std::stoul(cell);
        std::getline(ss, cell, ','); toId = std::stoul(cell);
        std::getline(ss, edgeType);

        // Auto-create any unknown node as a clean WORKSTATION
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

std::unique_ptr<Network> InputParser::buildNetwork(const std::string& patientZeroPath,
                                                    const std::string& hardenedPath,
                                                    const std::string& edgesPath) {
    auto network = std::make_unique<Network>();
    readPatientZero(patientZeroPath, *network);
    readEdges(edgesPath, *network);
    readHardenedNodes(hardenedPath, *network);
    return network;
}
