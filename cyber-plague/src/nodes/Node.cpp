#include "nodes/Node.h"
#include "policies/Policy.h"

// Set the node's ID and type, and start it as clean (not infected).
Node::Node(uint32_t id, NodeType type) : id(id), type(type), state(InfectionState::CLEAN) {}

Node::~Node() = default;

bool Node::isHardened() const {
    return state == InfectionState::HARDENED;
}

// Try to infect this node with the given malware payload.
// The node refuses infection in two situations:
//   1. It is hardened — protected by a firewall, so nothing gets through.
//   2. It is already infected with this exact same payload — no change needed.
// In every other case the node becomes infected and returns true to signal the change.
bool Node::receivePayload(const std::string& payload) {
    if (isHardened()) {
        return false; // firewall blocks all incoming malware
    } else if (state == InfectionState::INFECTED && acceptedPayload == payload) {
        return false; // already carrying this payload — nothing to update
    } else {
        state = InfectionState::INFECTED;
        acceptedPayload = payload;
        return true; // node is now infected
    }
}
