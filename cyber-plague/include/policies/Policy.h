#pragma once
#include <string>
#include <cstdint>
#include <vector>

// Represents a malware payload being spread through the network.
struct Payload {
    std::string name;           // the malware's name (e.g. "ransomware_v1")
    double exploitEfficiency;   // how effective this exploit is — used as a tiebreaker when two payloads compete
    uint32_t credentialLevel;   // the privilege level this malware carries (higher = more powerful)
    uint32_t originId;          // ID of the machine where this malware first appeared
};

// A Policy decides whether a machine will accept an incoming malware payload.
// Every node has one policy attached to it.
// Think of it like a spam filter — some machines accept everything,
// others only let through payloads that meet a minimum security threshold.
// This is a base class; the real behavior comes from StandardPolicy and HardenedPolicy.
class Policy {
public:
    virtual ~Policy() = default;

    // When multiple malware payloads arrive at a node at the same time, this picks the winner.
    // Returns the winning payload's name, or an empty string to reject all of them.
    virtual std::string resolveConflict(const std::vector<Payload>& candidates) = 0;

    // A quick check on each incoming payload before the winner is chosen.
    // Returns true if the payload is allowed through, false if it should be blocked immediately.
    virtual bool filterIncoming(const Payload& payload) = 0;
};
