#include "policies/HardenedPolicy.h"
#include <algorithm>

// Acts as a firewall gate: only payloads whose credentialLevel meets the node's
// minimum threshold are allowed to propagate. Everything below is dropped.
bool HardenedPolicy::filterIncoming(const Payload& payload) {
    return payload.credentialLevel >= minCredentialLevel_;  // drop anything below the threshold
}

// Among all candidates, only those that pass the credential threshold are
// considered. From that filtered set the winner is selected by a two-key
// priority: credentialLevel is the primary sort (higher is better), and
// exploitEfficiency breaks ties (also higher is better). std::max_element
// uses a less-than comparator, so returning a < b means b wins — the element
// for which no other element is "greater" under this ordering is the winner.
// If no candidate clears the threshold, the empty string signals no infection.
std::string HardenedPolicy::resolveConflict(const std::vector<Payload>& candidates) {
    std::vector<Payload> filtered;
    std::copy_if(candidates.begin(), candidates.end(), std::back_inserter(filtered),
        [this](const Payload& payload) { return filterIncoming(payload); });  // discard sub-threshold payloads

    if (filtered.empty()) {
        return "";  // no payload cleared the credential gate
    }

    auto winner = std::max_element(filtered.begin(), filtered.end(),
        [](const Payload& a, const Payload& b) {
            if (a.credentialLevel != b.credentialLevel) {
                return a.credentialLevel < b.credentialLevel;   // higher credential wins
            }
            return a.exploitEfficiency < b.exploitEfficiency;   // tiebreak: higher efficiency wins
        });

    return winner->name;
}
