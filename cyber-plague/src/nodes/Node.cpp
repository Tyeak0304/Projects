#include "nodes/Node.h"
#include "policies/Policy.h"


bool Node::isHardened() const{
    return state == InfectionState::HARDENED;
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
    if(isHardened()){
        return false;
    } else if(state == InfectionState::INFECTED && acceptedPayload == payload){
        return false;
    } else{
        state = InfectionState::INFECTED;
        acceptedPayload = payload;
        infectionPath.push_back(sourceId);
        return true;
    }
}

