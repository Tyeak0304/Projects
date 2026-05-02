#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "nodes/Node.h"

// Holds the entire simulated network — every machine and every connection between them.
// Think of it as a map of all computers and which other computers each one is linked to.
class Network {
public:
    // Add a new machine to the network.
    void addNode(std::unique_ptr<Node> node);

    // Connect two machines with a one-way link.
    // edgeType must be "provider", "peer", or "customer" — this determines
    // the direction infections spread and in what order.
    void addEdge(uint32_t fromId, uint32_t toId, const std::string& edgeType);

    // Look up a machine by its ID.
    // Returns nullptr if no machine with that ID exists.
    Node* getNode(uint32_t id);

    // Returns the total number of machines in the network.
    size_t size() const;

    // Checks the network for circular connections (e.g. A → B → C → A).
    // A circle would cause the simulation to run forever, so this must be
    // called before the simulator starts. Throws an error if a cycle is found.
    void validateTopology() const;

    // Returns a read-only view of all nodes so the simulator can loop over them.
    const std::unordered_map<uint32_t, std::unique_ptr<Node>>& getNodes() const { return nodes; }

private:
    // All machines stored by their ID for fast lookup.
    std::unordered_map<uint32_t, std::unique_ptr<Node>> nodes;

    // Internal helper that walks the network looking for cycles.
    // Uses a color system: 0 = not visited, 1 = currently visiting, 2 = fully done.
    bool hasCycleDFS(uint32_t id, std::unordered_map<uint32_t, int>& visited) const;
};
