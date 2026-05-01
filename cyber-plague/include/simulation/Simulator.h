#pragma once
#include <vector>
#include <memory>
#include "network/Network.h"

// Snapshot of a single node's infection state after the simulation completes
struct InfectionResult{
    uint32_t nodeId;
    bool acceptedPayload;                // true if this node ended up infected
    std::vector<uint32_t> infectionPath; // ordered route the infection traveled to reach this node
};

// Runs a BFS infection simulation over the network starting from all Patient Zero nodes
class Simulator{
    public:
        Simulator(std::unique_ptr<Network> network) : network_(std::move(network)) {}

        // Validates topology, runs BFS propagation, then collects results for all infected nodes
        void run();

        const std::vector<InfectionResult>& getResults() const;



    private:
        std::unique_ptr<Network> network_;
        std::vector<InfectionResult> results_;
        void propagate();
};
