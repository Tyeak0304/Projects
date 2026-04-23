#include "nodes/Node.h"
#include "policies/Policy.h"
#include <algorithm>

// TODO: Implement the Node constructor
//   1. Initialize id and type from parameters
//   2. Set state to InfectionState::CLEAN
//   3. Leave acceptedPayload as empty string
//   4. Leave infectionPath empty (it will be built during propagation)
//   5. Leave policy as nullptr (assigned externally by InputParser)
Node::Node(uint32_t id, NodeType type) {
    // TODO: Fill in constructor body
}

// TODO: Implement receivePayload
//   Purpose: attempt to infect this node with the given payload
//   Parameters:
//     - payload: name of the incoming malware payload
//     - sourceId: node ID that is sending the payload
//   Returns: true if the node became newly infected, false otherwise
//
//   Steps:
//   1. If this node isHardened(), return false immediately (policy blocks it)
//   2. If this node is already INFECTED with the same payload, return false
//   3. Otherwise, set state = InfectionState::INFECTED
//   4. Set acceptedPayload = payload
//   5. Append sourceId to infectionPath
//   6. Return true
bool Node::receivePayload(const std::string& payload, uint32_t sourceId) {
    // TODO: Fill in receivePayload body
    return false;
}
