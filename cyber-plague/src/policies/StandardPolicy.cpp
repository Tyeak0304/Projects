#include "policies/StandardPolicy.h"
#include <algorithm>

// TODO: Implement StandardPolicy::filterIncoming
//   Purpose: decide whether an incoming payload is allowed through at all
//   A StandardPolicy node is not hardened, so it accepts everything
//   Steps:
//   1. Always return true (no filtering on a standard node)
bool StandardPolicy::filterIncoming(const Payload& payload) {
    // TODO: Fill in filterIncoming body
    return false;
}

// TODO: Implement StandardPolicy::resolveConflict
//   Purpose: when a node receives multiple competing payloads, pick the winner
//   Selection priority (analogous to BGP path selection):
//     1. Highest credentialLevel wins
//     2. Tiebreak: highest exploitEfficiency wins
//     3. If candidates is empty, return "" (reject all)
//   Hint: use std::max_element with a lambda comparator
//   Returns: the winning payload's name, or "" if candidates is empty
std::string StandardPolicy::resolveConflict(const std::vector<Payload>& candidates) {
    // TODO: Fill in resolveConflict body
    return "";
}
