#pragma once
#include "Policy.h"

// TODO: Define HardenedPolicy class (inherits Policy)
// TODO: Add constructor with minCredentialLevel parameter (default 2)
// TODO: Implement resolveConflict() — IDS/Firewall conflict resolution
// TODO: Implement filterIncoming() — drop payloads below credential threshold
// TODO: Add private minCredentialLevel_ field

class HardenedPolicy : public Policy{
    private:
        uint32_t minCredentialLevel;

    public:
        HardenedPolicy(uint32_t minCredentialLevel = 2);
        std::string resolveConflict(const std::vector<Payload>& candidates) override;
        bool filterIncoming(const Payload& payload) override;
};