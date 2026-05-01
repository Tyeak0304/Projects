#pragma once
#include "Policy.h"

// TODO: Define StandardPolicy class (inherits Policy)
// TODO: Implement resolveConflict() — prefer highest credential level, then exploit efficiency
// TODO: Implement filterIncoming() — accept all payloads by default

class StandardPolicy : public Policy{
    public:
        std::string resolveConflict(const Payload& parload1, const Payload& payload2) override;
        bool filterIncoming(const Payload& payload) override;
};