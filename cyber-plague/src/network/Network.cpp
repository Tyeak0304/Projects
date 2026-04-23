#include "network/Network.h"
#include <stdexcept>

// TODO: Implement Network::addNode
//   Purpose: insert a node into the graph
//   Steps:
//   1. Use nodes.emplace(node->id, std::move(node)) to insert
//      (emplace avoids a copy and takes ownership of the unique_ptr)
void Network::addNode(std::unique_ptr<Node> node) {
    // TODO: Fill in addNode body
}

// TODO: Implement Network::addEdge
//   Purpose: create a directed relationship between two nodes
//   Parameters:
//     - fromId: the node initiating the relationship
//     - toId:   the neighbor node
//     - edgeType: "provider", "peer", or "customer"
//   Steps:
//   1. Use getNode(fromId) to retrieve the source node pointer
//   2. If the pointer is nullptr, return early (node doesn't exist)
//   3. Check edgeType and push toId into the correct adjacency list:
//        "provider"  -> from->providers
//        "peer"      -> from->peers
//        "customer"  -> from->customers
void Network::addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType) {
    // TODO: Fill in addEdge body
}

// TODO: Implement Network::getNode
//   Purpose: look up a node by ID
//   Steps:
//   1. Use nodes.find(id) to search the map
//   2. If found, return it->second.get() (raw pointer from unique_ptr)
//   3. If not found (it == nodes.end()), return nullptr
Node* Network::getNode(uint32_t id) {
    // TODO: Fill in getNode body
    return nullptr;
}

// TODO: Implement Network::validateTopology
//   Purpose: detect cycles in the provider/customer directed graph
//   A cycle would cause infinite propagation and must be caught before simulation
//   Steps:
//   1. Create an unordered_map<uint32_t, int> visited, all initialized to 0
//      (0 = unvisited, 1 = in current DFS stack, 2 = fully processed)
//   2. For each node in nodes, if visited[id] == 0, call hasCycleDFS(id, visited)
//   3. If hasCycleDFS returns true, throw std::runtime_error with a descriptive message
void Network::validateTopology() const {
    // TODO: Fill in validateTopology body
}

// TODO: Implement Network::hasCycleDFS (private helper)
//   Purpose: recursive DFS that detects back edges (cycles)
//   Steps:
//   1. Mark visited[nodeId] = 1 (currently on the DFS stack)
//   2. Get the current node with nodes.at(nodeId).get()
//   3. Iterate over the node's providers AND customers as neighbors
//      (peers are lateral and don't form a hierarchy, so skip them)
//   4. For each neighbor ID:
//        a. If visited[neighbor] == 1 -> cycle found, return true
//        b. If visited[neighbor] == 0 -> recurse: if hasCycleDFS returns true, return true
//   5. Mark visited[nodeId] = 2 (fully processed, safe)
//   6. Return false
bool Network::hasCycleDFS(uint32_t nodeId,
                           std::unordered_map<uint32_t, int>& visited) const {
    // TODO: Fill in hasCycleDFS body
    return false;
}
