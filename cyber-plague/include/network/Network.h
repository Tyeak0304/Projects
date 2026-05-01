#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "nodes/Node.h"

// Directed graph of nodes connected by three relationship types: provider, peer, customer.
// Ownership of all nodes lives here via unique_ptr.
class Network{
    public:
        // Takes ownership of the node and indexes it by its ID
        void addNode(std::unique_ptr<Node> node);

        // Creates a directed edge from fromId to toId; edgeType must be "provider", "peer", or "customer"
        void addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType);

        // Returns a raw (non-owning) pointer to the node, or nullptr if the ID is not found
        Node* getNode(uint32_t id);

        size_t size() const;

        // Walks the provider/customer hierarchy with DFS; throws std::runtime_error if a cycle is found.
        // Call this before running the simulator — a cycle would cause infinite BFS propagation.
        void validateTopology() const;

    private:
        std::unordered_map<uint32_t, std::unique_ptr<Node>> nodes;

        // DFS helper using a three-color mark (0=unvisited, 1=on stack, 2=done).
        // Returns true if a cycle is reachable from nodeId.
        bool hasCycleDFS(uint32_t id, std::unordered_map<uint32_t, int>& visited) const;
};
