#include "io/InputParser.h"
#include "nodes/Node.h"
#include "policies/StandardPolicy.h"
#include "policies/HardenedPolicy.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// TODO: Implement InputParser::buildNetwork
//   Purpose: parse both CSVs and return a fully constructed Network
//
//   --- Step 1: Read hardened_nodes.csv ---
//   Format: node_id,firewall_level
//   1. Open hardenedPath with std::ifstream, throw std::runtime_error if it fails
//   2. Skip the header line using std::getline
//   3. For each remaining line:
//        a. Parse node_id (uint32_t) and firewall_level (int) using stringstream + getline(ss, cell, ',')
//        b. Store in a std::unordered_map<uint32_t, int> hardenedMap
//
//   --- Step 2: Read patient_zero.csv ---
//   Format: node_id,payload_name,exploit_efficiency,credential_level
//   1. Open patientZeroPath with std::ifstream, throw std::runtime_error if it fails
//   2. Skip the header line
//   3. For each remaining line:
//        a. Parse all four fields using stringstream + getline(ss, cell, ',')
//        b. Create a new Node: auto node = std::make_unique<Node>(id, NodeType::WORKSTATION)
//           (All Patient Zero nodes are workstations in this simplified model)
//        c. Check hardenedMap: if this id is present, assign a HardenedPolicy,
//           otherwise assign a StandardPolicy
//           Hint: node->policy = std::make_unique<HardenedPolicy>(hardenedMap[id])
//        d. Mark it as Patient Zero by setting:
//              node->state = InfectionState::INFECTED
//              node->acceptedPayload = payloadName
//              node->infectionPath = { id }  (starts its own path)
//        e. Call network->addNode(std::move(node))
//
//   --- Step 3: Return the network ---
//   4. Return network (use std::move since it's a unique_ptr)
//
//   Note: In a full implementation you would also parse edge relationships
//   from a separate topology CSV. That extension is left as a TODO.
std::unique_ptr<Network> InputParser::buildNetwork(const std::string& patientZeroPath,
                                                    const std::string& hardenedPath) {
    auto network = std::make_unique<Network>();

    // TODO: Step 1 — parse hardenedPath into hardenedMap
        std::unordered_map<uint32_t, int> hardenedMap;
        std::ifstream hardenedFile(hardenedPath);
        if (!hardenedFile.is_open()) {
            throw std::runtime_error("Failed to open hardened nodes file: " + hardenedPath);
        }
        std::string line;
        std::getline(hardenedFile, line); // Skip header
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

    // TODO: Step 2 — parse patientZeroPath, create nodes, assign policies, add to network
    std::ifstream patientZeroPathFile(patientZeroPath);
    if (!patientZeroPathFile.is_open()) {
        throw std::runtime_error("Failed to open patient zero file: " + patientZeroPath);
    }
    std::getline(patientZeroPathFile, line); // Skip header
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

        auto node = std::make_unique<Node> (nodeId, NodeType::WORKSTATION);
        if (hardenedMap.count(nodeId)){
            node->policy = std::make_unique<HardenedPolicy>(hardenedMap[nodeId]);
        } else {
            node->policy = std::make_unique<StandardPolicy>();
        }
        node->state = InfectionState::INFECTED;
        node->acceptedPayload = payloadName;
        node->infectionPath = { nodeId };
        network->addNode(std::move(node));
    }
    patientZeroPathFile.close();

    // TODO: Step 3 — return network
    return network;
}
