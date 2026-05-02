#pragma once
#include "Policy.h"

// The policy for protected machines (firewalls, IDS systems).
// A hardened node rejects any malware that isn't powerful enough —
// like a door that only opens with a key of the right strength.
class HardenedPolicy : public Policy {
private:
    uint32_t minCredentialLevel_; // payloads below this privilege level are blocked entirely

public:
    // The default threshold of 2 means only elevated-privilege payloads can get through.
    // A higher threshold makes the node harder to infect.
    HardenedPolicy(uint32_t minCredentialLevel = 2) : minCredentialLevel_(minCredentialLevel) {}

    // Filters out weak payloads first, then picks the strongest one that passed.
    // Returns an empty string if no payload met the minimum threshold.
    std::string resolveConflict(const std::vector<Payload>& candidates) override;

    // Returns true only if the payload's credentialLevel is high enough to pass this node's firewall.
    bool filterIncoming(const Payload& payload) override;
};
