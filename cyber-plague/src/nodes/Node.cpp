#include "nodes/Node.h"
#include "policies/Policy.h"

Node::Node(uint32_t id, NodeType type) : id(id), type(type), state(InfectionState::CLEAN) {}

Node::~Node() = default;

bool Node::isHardened() const{
    return state == InfectionState::HARDENED;
}

// Attempts to infect this node with the given payload sent from sourceId.
// Blocked if the node is hardened or already carrying the exact same payload.
bool Node::receivePayload(const std::string& payload, uint32_t sourceId) {
    if(isHardened()){
        return false;
    } else if(state == InfectionState::INFECTED && acceptedPayload == payload){
        return false; // already infected with this exact payload; no state change needed
    } else{
        state = InfectionState::INFECTED;
        acceptedPayload = payload;
        infectionPath.push_back(sourceId); // record which node sent the infection
        return true;
    }
}
