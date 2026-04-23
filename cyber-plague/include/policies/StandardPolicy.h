#pragma once
#include "Policy.h"

// Default node policy: accepts payloads, prefers highest credential level,
// then highest exploit efficiency as tiebreaker.
class StandardPolicy : public Policy {
public:
    std::string resolveConflict(const std::vector<Payload>& candidates) override;
    bool filterIncoming(const Payload& payload) override;
};
