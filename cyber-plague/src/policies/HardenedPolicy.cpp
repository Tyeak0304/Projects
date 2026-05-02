#include "policies/HardenedPolicy.h"
#include <algorithm>

// A hardened node acts like a firewall — it only allows through payloads
// that are powerful enough (credentialLevel >= the node's minimum threshold).
// Anything weaker is dropped before it even gets considered.
bool HardenedPolicy::filterIncoming(const Payload& payload) {
    return payload.credentialLevel >= minCredentialLevel_; // too weak? blocked.
}

// From all competing payloads, first discard any that don't clear the firewall,
// then pick the strongest survivor using the same two rules as StandardPolicy:
//   1. Highest credentialLevel wins.
//   2. Highest exploitEfficiency breaks ties.
// If every payload was blocked by the filter, return an empty string — infection prevented.
std::string HardenedPolicy::resolveConflict(const std::vector<Payload>& candidates) {
    // Keep only payloads that pass the firewall check.
    std::vector<Payload> filtered;
    std::copy_if(candidates.begin(), candidates.end(), std::back_inserter(filtered),
        [this](const Payload& payload) { return filterIncoming(payload); });

    if (filtered.empty()) {
        return ""; // all payloads were blocked — this node stays safe
    }

    // Pick the strongest payload from those that cleared the firewall.
    auto winner = std::max_element(filtered.begin(), filtered.end(),
        [](const Payload& a, const Payload& b) {
            if (a.credentialLevel != b.credentialLevel) {
                return a.credentialLevel < b.credentialLevel; // higher credential wins
            }
            return a.exploitEfficiency < b.exploitEfficiency; // tiebreak: higher efficiency wins
        });

    return winner->name;
}
