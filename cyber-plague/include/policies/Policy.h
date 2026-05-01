#pragma once
#include <string>
#include <cstdint>

// TODO: Define Payload struct (name, exploitEfficiency, credentialLevel, originId)
// TODO: Define abstract Policy class
// TODO: Add virtual destructor
// TODO: Add pure virtual resolveConflict() — returns winning payload name or "" to reject all
// TODO: Add pure virtual filterIncoming() — returns true if payload passes IDS filter

struct Payload{
    std::string name;
    double exploitEfficiency;
    uint32_t credentialLevel;
    uint32_t originId;
};

class Policy{
    public:
        virtual ~Policy() = default;

        virtual std::string resolveConflict(const Payload& payload1, const Payload& payload2) =0;

        virtual bool filterIncoming(const Payload& payload) =0;
};

