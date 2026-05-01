#pragma once
#include <vector>
#include <memory>
#include "network/Network.h"

// TODO: Define InfectionResult struct (nodeId, acceptedPayload, infectionPath)
// TODO: Define Simulator class
// TODO: Add constructor taking unique_ptr<Network>
// TODO: Implement run()
// TODO: Implement getResults()
// TODO: Add private network_ and results_ fields
// TODO: Implement propagate() — BFS from all Patient Zero nodes

struct InfectionResult{
    uint32_t nodeId;
    bool acceptedPayload;
    std::vector<uint32_t> infectionPath;

};

class Simulator{
    public:
        Simulator(std::unique_ptr<Network> network);
        void run();
        const std::vector<InfectionResult>& getResults() const;
        void propagate();
    private:
        std::unique_ptr<Network> network_;
        std::vector<InfectionResult> results_;
        
};
