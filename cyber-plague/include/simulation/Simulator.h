#pragma once
#include <vector>
#include <memory>
#include "network/Network.h"

struct InfectionResult {
    uint32_t nodeId;
    std::string acceptedPayload;
    std::vector<uint32_t> infectionPath;
};

class Simulator {
public:
    explicit Simulator(std::unique_ptr<Network> network);

    void run();
    const std::vector<InfectionResult>& getResults() const { return results_; }

private:
    std::unique_ptr<Network> network_;
    std::vector<InfectionResult> results_;

    // BFS propagation starting from all Patient Zero nodes
    void propagate();
};
