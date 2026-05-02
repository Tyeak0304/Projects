#include "policies/StandardPolicy.h"
#include <algorithm>

// An unprotected node has no filter at all — it lets every incoming payload through.
// This simulates a computer with no antivirus or firewall installed.
bool StandardPolicy::filterIncoming(const Payload& payload) {
    return true; // no checks — everything is allowed
}

// When multiple malware payloads are trying to infect this node at the same time,
// this picks the strongest one using two rules:
//   1. The payload with the highest credentialLevel wins.
//   2. If two payloads have equal credentialLevel, the one with higher exploitEfficiency wins.
// If there are no candidates at all, return an empty string to signal no infection.
std::string StandardPolicy::resolveConflict(const std::vector<Payload>& candidates) {
    if (candidates.empty()) {
        return ""; // nothing to choose from
    }

    // std::max_element finds the "greatest" element using the comparison function below.
    // The comparison returns true when a should lose to b, so the element
    // that no other element beats is declared the winner.
    auto winner = std::max_element(candidates.begin(), candidates.end(),
        [](const Payload& a, const Payload& b) {
            if (a.credentialLevel != b.credentialLevel) {
                return a.credentialLevel < b.credentialLevel; // higher credential wins
            }
            return a.exploitEfficiency < b.exploitEfficiency; // tiebreak: higher efficiency wins
        });

    return winner->name;
}
