#pragma once
#include <string>
#include <vector>

struct Payload {
    std::string name;
    int exploitEfficiency;   // 0-100
    int credentialLevel;     // 0=user, 1=admin, 2=root/SYSTEM
    uint32_t originId;
};

class Policy {
public:
    virtual ~Policy() = default;

    // Conflict resolution: returns the winning payload name, or "" to reject all
    virtual std::string resolveConflict(const std::vector<Payload>& candidates) = 0;

    // IDS filter: returns true if the payload passes (is not dropped)
    virtual bool filterIncoming(const Payload& payload) = 0;
};
