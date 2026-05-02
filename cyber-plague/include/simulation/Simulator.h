#pragma once
#include <vector>
#include <set>
#include <memory>
#include "network/Network.h"

// Stores the final infection record for one node after the simulation ends.
struct InfectionResult {
    uint32_t nodeId;
    std::string acceptedPayload;         // which malware infected this node
    std::vector<uint32_t> infectionPath; // the exact route the malware took to get here (list of node IDs)
};

// Runs the infection simulation across the entire network.
// Starting from all patient zero nodes, it spreads the infection outward
// one "wave" at a time — like ripples in a pond — until no more nodes can be reached.
// This wave-by-wave approach is called BFS (breadth-first search).
class Simulator {
public:
    Simulator(std::unique_ptr<Network> network, bool verbose = false, bool animate = false)
        : network_(std::move(network)), verbose_(verbose), animate_(animate) {}

    // Runs the full simulation: checks the network for problems, spreads the infection,
    // then records which nodes got infected and how.
    void run();

    // Returns the list of all infected nodes after run() has completed.
    const std::vector<InfectionResult>& getResults() const;

private:
    std::unique_ptr<Network> network_; // the network being simulated
    std::vector<InfectionResult> results_; // collected results — one entry per infected node
    bool verbose_;  // if true, prints a detailed blow-by-blow of every infection attempt
    bool animate_;  // if true, redraws the network map on screen after each wave

    // The main infection loop — spreads from wave to wave until nothing new can be infected.
    void propagate();

    // Draws a color-coded map of the network in the terminal showing which nodes are
    // clean (white), infected (colored by payload), or hardened (green).
    void drawMap(int wave, const std::set<uint32_t>& newlyInfected) const;
};
