#pragma once
#include "Policy.h"

// Default policy for unprotected nodes — accepts all payloads and picks the strongest one.
class StandardPolicy : public Policy{
    public:
        // Picks the candidate with the highest credentialLevel; breaks ties by exploitEfficiency.
        // Returns "" if candidates is empty.
        std::string resolveConflict(const std::vector<Payload>& candidates) override;

        // Always returns true — standard nodes have no incoming filter.
        bool filterIncoming(const Payload& payload) override;
};
