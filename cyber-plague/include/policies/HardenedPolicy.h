#pragma once
#include "Policy.h"

// Applied to firewall/IDS nodes — rejects any payload that doesn't meet the minimum credential threshold.
class HardenedPolicy : public Policy{
    private:
        uint32_t minCredentialLevel_;   // payloads with credentialLevel below this value are dropped

    public:
        // Default threshold of 2 means only elevated-privilege payloads pass the filter
        HardenedPolicy(uint32_t minCredentialLevel = 2) : minCredentialLevel_(minCredentialLevel) {}

        // Filters candidates through filterIncoming, then picks the winner by credentialLevel (primary)
        // and exploitEfficiency (tiebreaker); returns "" if no candidate clears the threshold
        std::string resolveConflict(const std::vector<Payload>& candidates) override;

        // Returns true only if payload.credentialLevel >= minCredentialLevel_
        bool filterIncoming(const Payload& payload) override;
};
