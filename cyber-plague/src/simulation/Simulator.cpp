#include "simulation/Simulator.h"
#include <queue>
#include <iostream>

// TODO: Implement the Simulator constructor
//   Steps:
//   1. Move the network parameter into network_ using std::move
Simulator::Simulator(std::unique_ptr<Network> network) {
    // TODO: Fill in constructor body
}

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
    // TODO: Fill in run body
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
}
