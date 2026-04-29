#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "nodes/Node.h"

// TODO: Define Network class
// TODO: Add nodes map (uint32_t -> unique_ptr<Node>)
// TODO: Implement addNode()
// TODO: Implement addEdge() with edgeType: "provider", "peer", or "customer"
// TODO: Implement getNode() and size()
// TODO: Implement validateTopology() with DFS cycle detection
// TODO: Add private hasCycleDFS() helper

class Network{
    public:
        void addNode(std::unique_ptr<Node> node);
        void addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType);
        Node* getNode(uint32_t id);
        size_t size() const { return nodes.size();}
        void validateTopologyy() const;
    private:
        std::unordered_map<uint32_t, std::unique_ptr<Node>> nodes;
        bool hasCycleDFS(uint32_t id, std::unordered_map<uint32_t, int>& visited) const;
};
