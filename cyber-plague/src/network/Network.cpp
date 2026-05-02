#include "network/Network.h"
#include <stdexcept>

size_t Network::size() const { return nodes.size(); }

// Store the node in the map using its ID as the key so we can look it up quickly later.
void Network::addNode(std::unique_ptr<Node> node) {
    nodes.emplace(node->id, std::move(node));
}

// Create a one-way connection from one node to another.
// The edge type controls which neighbor list the destination gets added to,
// which in turn determines the order infections spread:
//   providers first → peers second → customers last.
void Network::addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType) {
    Node* fromNode = getNode(fromId);
    if (!fromNode) {
        return; // the source node doesn't exist in the network — skip this edge
    }

    if (edgeType == "provider") {
        fromNode->providers.push_back(toId);
    } else if (edgeType == "peer") {
        fromNode->peers.push_back(toId);
    } else if (edgeType == "customer") {
        fromNode->customers.push_back(toId);
    }
}

// Find and return a node by its ID.
// Returns nullptr if no node with that ID exists — callers must check for this.
Node* Network::getNode(uint32_t id) {
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

// Walk the entire network looking for circular connections (e.g. A → B → C → A).
// If a circle exists the BFS simulator would loop forever, so we catch it here
// before the simulation starts and throw an error with the offending node's ID.
void Network::validateTopology() const {
    std::unordered_map<uint32_t, int> visited;
    for (const auto& pair : nodes) {
        uint32_t id = pair.first;
        if (visited[id] == 0) {
            if (hasCycleDFS(id, visited)) {
                throw std::runtime_error("Cycle detected in network topology involving node ID: " + std::to_string(id));
            }
        }
    }
}

// Walks the network depth-first from a given node, looking for cycles.
// Each node gets a color to track its visit state:
//   0 = not yet visited
//   1 = currently being visited (still on the path we're exploring)
//   2 = fully processed — no cycle was found through this node
// If we reach a neighbor that is still being visited (color 1), we've looped back — that's a cycle.
// Only provider and customer edges are checked because those form the hierarchy;
// peer edges are lateral and can't create a problematic loop.
bool Network::hasCycleDFS(uint32_t nodeId, std::unordered_map<uint32_t, int>& visited) const {
    visited[nodeId] = 1; // mark as currently being explored
    const Node* currentNode = nodes.at(nodeId).get();

    for (uint32_t neighborId : currentNode->providers) {
        if (visited[neighborId] == 1) return true; // looped back to a node we haven't finished yet
        if (visited[neighborId] == 0 && hasCycleDFS(neighborId, visited)) return true;
    }
    for (uint32_t neighborId : currentNode->customers) {
        if (visited[neighborId] == 1) return true;
        if (visited[neighborId] == 0 && hasCycleDFS(neighborId, visited)) return true;
    }

    visited[nodeId] = 2; // fully processed — no cycle found through this node
    return false;
}
