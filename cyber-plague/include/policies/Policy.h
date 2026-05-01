#pragma once
#include <string>
#include <cstdint>
#include <vector>

// A malware payload being propagated through the network
struct Payload{
    std::string name;
    double exploitEfficiency;   // effectiveness of the exploit; used as a tiebreaker during conflict resolution
    uint32_t credentialLevel;   // privilege level required to propagate (higher = more powerful)
    uint32_t originId;          // ID of the node that first introduced this payload
};

// Abstract base class — each node holds one Policy that governs how it handles incoming payloads.
// Subclasses implement the filtering and conflict-resolution logic.
class Policy{
    public:
        virtual ~Policy() = default;

        // Picks the winning payload from a set of competing candidates.
        // Returns the winner's name, or "" to reject all candidates.
        virtual std::string resolveConflict(const std::vector<Payload>& candidates) = 0;

        // Returns true if the payload is allowed past this node's filter (IDS/firewall check).
        // Called before resolveConflict — payloads that fail here are never considered.
        virtual bool filterIncoming(const Payload& payload) = 0;
};
