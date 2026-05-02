#pragma once
#include "Policy.h"

// The default policy for unprotected machines.
// A standard node has no firewall — it accepts any malware that arrives,
// like a computer with no antivirus software installed.
class StandardPolicy : public Policy {
public:
    // When multiple payloads compete, pick the one with the highest credentialLevel.
    // If two payloads are tied on credentialLevel, exploitEfficiency breaks the tie.
    // Returns an empty string if there are no candidates.
    std::string resolveConflict(const std::vector<Payload>& candidates) override;

    // Always returns true — unprotected nodes don't block anything.
    bool filterIncoming(const Payload& payload) override;
};
