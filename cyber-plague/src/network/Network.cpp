#include "network/Network.h"
#include <stdexcept>

size_t Network::size() const { return nodes.size(); }

// Takes ownership of the node and stores it indexed by node->id
void Network::addNode(std::unique_ptr<Node> node) {
    nodes.emplace(node->id, std::move(node));
}

// Records a directed relationship from fromId toward toId.
// edgeType controls which adjacency list receives the neighbor entry.
void Network::addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType) {
    Node* fromNode = getNode(fromId);
    if (!fromNode){
        return; // silently ignore edges that reference an unknown source node
    }

    if(edgeType == "provider"){
        fromNode->providers.push_back(toId);
    } else if(edgeType == "peer"){
        fromNode->peers.push_back(toId);
    } else if(edgeType == "customer"){
        fromNode->customers.push_back(toId);
    }
}

// Returns a raw (non-owning) pointer into the map's unique_ptr, or nullptr if absent
Node* Network::getNode(uint32_t id) {
    auto it = nodes.find(id);
    if(it != nodes.end()){
        return it->second.get();
    }else{
        return nullptr;
    }
}

// Detects cycles in the provider/customer hierarchy via DFS.
// Peers are lateral relationships and excluded — only the hierarchical edges can form a cycle.
void Network::validateTopology() const {
    std::unordered_map<uint32_t, int> visited;
    for (const auto& pair : nodes) {
        uint32_t id = pair.first;
        if (visited[id] == 0){
            if(hasCycleDFS(id, visited)){
                throw std::runtime_error("Cycle detected in network topology involving node ID: " + std::to_string(id));
            }
        }
    }
}

// Three-color DFS: 0=unvisited, 1=currently on the recursion stack, 2=fully processed.
// Encountering a neighbor marked 1 means we've looped back — a cycle exists.
bool Network::hasCycleDFS(uint32_t nodeId, std::unordered_map<uint32_t, int>& visited) const {
    visited[nodeId] = 1; // push onto stack
    const Node* currentNode = nodes.at(nodeId).get();

    for(uint32_t neighborId : currentNode->providers){
        if(visited[neighborId] == 1) return true;
        if(visited[neighborId] == 0 && hasCycleDFS(neighborId, visited)) return true;
    }
    for(uint32_t neighborId : currentNode->customers){
        if(visited[neighborId] == 1) return true;
        if(visited[neighborId] == 0 && hasCycleDFS(neighborId, visited)) return true;
    }

    visited[nodeId] = 2; // pop from stack — this subtree is cycle-free
    return false;
}
