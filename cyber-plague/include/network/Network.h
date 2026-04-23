#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "nodes/Node.h"

class Network {
public:
    std::unordered_map<uint32_t, std::unique_ptr<Node>> nodes;

    void addNode(std::unique_ptr<Node> node);

    // edgeType: "provider", "peer", or "customer"
    void addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType);

    Node* getNode(uint32_t id);
    size_t size() const { return nodes.size(); }

    // DFS cycle detection — throws std::runtime_error on cycle
    void validateTopology() const;

private:
    bool hasCycleDFS(uint32_t nodeId,
                     std::unordered_map<uint32_t, int>& visited) const;
};
