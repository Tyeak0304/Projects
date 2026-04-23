#include "policies/HardenedPolicy.h"
#include <algorithm>

// TODO: Implement the HardenedPolicy constructor
//   Purpose: store the minimum credential level required to pass this node's IDS
//   Steps:
//   1. Initialize minCredentialLevel_ from the parameter
HardenedPolicy::HardenedPolicy(int minCredentialLevel) {
    // TODO: Fill in constructor body
}

// TODO: Implement HardenedPolicy::filterIncoming
//   Purpose: IDS/Firewall logic — drop payloads that don't meet the threshold
//   This is the ROV equivalent: it validates the "route" before accepting it
//   Steps:
//   1. Return true ONLY if payload.credentialLevel >= minCredentialLevel_
//   2. Otherwise return false (payload is dropped, propagation blocked)
bool HardenedPolicy::filterIncoming(const Payload& payload) {
    // TODO: Fill in filterIncoming body
    return false;
}

// TODO: Implement HardenedPolicy::resolveConflict
//   Purpose: among payloads that passed filterIncoming, pick the best one
//   A hardened node is very restrictive — even among passing payloads, prefer
//   the one with the highest credentialLevel first, then exploitEfficiency
//   Steps:
//   1. Filter candidates vector to only those passing filterIncoming
//      Hint: use std::copy_if into a new vector
//   2. If the filtered list is empty, return ""
//   3. Otherwise use std::max_element (same comparator as StandardPolicy)
//      and return the winner's name
std::string HardenedPolicy::resolveConflict(const std::vector<Payload>& candidates) {
    // TODO: Fill in resolveConflict body
    return "";
}
