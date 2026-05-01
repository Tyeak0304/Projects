#pragma once
#include <vector>
#include <set>
#include <memory>
#include "network/Network.h"

// Snapshot of a single node's infection state after the simulation completes
struct InfectionResult{
    uint32_t nodeId;
    std::string acceptedPayload;         // name of the payload infecting this node
    std::vector<uint32_t> infectionPath; // ordered route the infection traveled to reach this node
};

// Runs a BFS infection simulation over the network starting from all Patient Zero nodes
class Simulator{
    public:
        Simulator(std::unique_ptr<Network> network, bool verbose = false, bool animate = false)
            : network_(std::move(network)), verbose_(verbose), animate_(animate) {}

        // Validates topology, runs BFS propagation, then collects results for all infected nodes
        void run();

        const std::vector<InfectionResult>& getResults() const;

    private:
        std::unique_ptr<Network> network_;
        std::vector<InfectionResult> results_;
        bool verbose_;
        bool animate_;

        void propagate();
        void drawMap(int wave, const std::set<uint32_t>& newlyInfected) const;
};
