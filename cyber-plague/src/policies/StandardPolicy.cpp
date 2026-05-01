#include "policies/StandardPolicy.h"
#include <algorithm>

// A standard (non-hardened) node imposes no credential gate — every incoming
// payload is accepted regardless of its credentialLevel. This mirrors an
// unpatched host that runs whatever code arrives.
bool StandardPolicy::filterIncoming(const Payload& payload) {
    return true;  // no threshold check; all payloads pass
}

// Selects the winning payload from all candidates using a two-key priority:
// credentialLevel is the primary sort (higher is better), and exploitEfficiency
// breaks ties (also higher is better). std::max_element uses a less-than
// comparator, so returning a < b means b wins — the element for which no other
// element is "greater" under this ordering is the winner.
// If candidates is empty, the empty string signals no infection.
std::string StandardPolicy::resolveConflict(const std::vector<Payload>& candidates) {
    if (candidates.empty()) {
        return "";  // nothing to propagate
    }

    auto winner = std::max_element(candidates.begin(), candidates.end(),
        [](const Payload& a, const Payload& b) {
            if (a.credentialLevel != b.credentialLevel) {
                return a.credentialLevel < b.credentialLevel;   // higher credential wins
            }
            return a.exploitEfficiency < b.exploitEfficiency;   // tiebreak: higher efficiency wins
        });

    return winner->name;
}
