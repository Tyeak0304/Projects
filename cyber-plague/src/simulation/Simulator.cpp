#include "simulation/Simulator.h"
#include <queue>
#include <iostream>

// TODO: Implement Simulator::run
//   Purpose: entry point for the full simulation
//   Steps:
//   1. Call network_->validateTopology() — throws on cycle (let it propagate)
//   2. Print a status message: total node count via network_->size()
//   3. Call propagate() to run the BFS infection spread
//   4. After propagate() finishes, iterate over all nodes in network_->nodes
//      For each node that is INFECTED:
//        a. Build an InfectionResult {node.id, node.acceptedPayload, node.infectionPath}
//        b. Push it into results_
void Simulator::run() {
    network_->validateTopology();
    std::cout << "Simulation has started... Total nodes: " << network_->size() << std::endl;
    propagate();
    for (const auto& [id, node] : network_->getNodes()) {
        if (node->state == InfectionState::INFECTED) {
            InfectionResult result{node->id, true, node->infectionPath};
            results_.push_back(result);
        }
    }
}


const std::vector<InfectionResult>& Simulator::getResults() const{
    return results_;
}

// TODO: Implement Simulator::propagate (private)
//   Purpose: BFS spread of malware starting from all Patient Zero nodes
//
//   Setup:
//   1. Create a std::queue<uint32_t> bfsQueue
//   2. Iterate over all nodes in network_->nodes
//      If a node is already INFECTED (set by InputParser as Patient Zero),
//      push its id into bfsQueue
//
//   BFS loop (while bfsQueue is not empty):
//   3. Pop the front node id (current = bfsQueue.front(); bfsQueue.pop())
//   4. Get the current Node* via network_->getNode(current)
//   5. If the node pointer is null or NOT infected, continue to next iteration
//
//   Propagation order — try neighbors in this order: providers, then peers, then customers
//   (Up -> Across -> Down, per Propagation Rank logic)
//   6. For each neighbor list (providers, peers, customers):
//        For each neighbor id in the list:
//          a. Get the neighbor Node* via network_->getNode(neighborId)
//          b. If neighbor is null or already INFECTED, skip
//          c. Call neighbor->receivePayload(currentNode->acceptedPayload, current)
//          d. If receivePayload returns true (newly infected), push neighborId into bfsQueue
void Simulator::propagate() {
    // TODO: Fill in propagate body
    std::queue<uint32_t> bfsQueue;
    for(const auto& [id, node] : network_->getNodes()){
        if(node->state == InfectionState::INFECTED){
            bfsQueue.push(id);
        }
    }
    while(!bfsQueue.empty()){
        uint32_t current = bfsQueue.front();
        bfsQueue.pop();
        Node* currentNode = network_->getNode(current);
        if(currentNode == nullptr || currentNode->state != InfectionState::INFECTED){
            continue;
        }
        for(uint32_t neighborId : currentNode->providers){
            Node* neighborNode = network_->getNode(neighborId);
            if(neighborNode == nullptr || neighborNode->state == InfectionState::INFECTED){
                continue;
            }
            if(neighborNode->receivePayload(currentNode->acceptedPayload, current)){
                bfsQueue.push(neighborId);
            }
        }
        for(uint32_t neighborId : currentNode->peers){
            Node* neighborNode = network_->getNode(neighborId);
            if(neighborNode == nullptr || neighborNode->state == InfectionState::INFECTED){
                continue;
            }
            if(neighborNode->receivePayload(currentNode->acceptedPayload, current)){
                bfsQueue.push(neighborId);
            }
        }
        for(uint32_t neighborId : currentNode->customers){
            Node* neighborNode = network_->getNode(neighborId);
            if(neighborNode == nullptr || neighborNode->state == InfectionState::INFECTED){
                continue;
            }
            if(neighborNode->receivePayload(currentNode->acceptedPayload, current)){
                bfsQueue.push(neighborId);
            }
        }
    }
}


