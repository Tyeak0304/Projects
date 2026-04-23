#pragma once
#include "Policy.h"

// Security-hardened node (ROV equivalent).
// Runs IDS/Firewall logic: drops payloads below a credential threshold
// and blocks propagation to neighbors.
class HardenedPolicy : public Policy {
public:
    explicit HardenedPolicy(int minCredentialLevel = 2);
    std::string resolveConflict(const std::vector<Payload>& candidates) override;
    bool filterIncoming(const Payload& payload) override;

private:
    int minCredentialLevel_;
};
