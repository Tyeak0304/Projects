#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

// TODO: Define NodeType enum (WORKSTATION, SERVER, DOMAIN_CONTROLLER, IOT_DEVICE, GATEWAY, FIREWALL)
// TODO: Define InfectionState enum (CLEAN, INFECTED, HARDENED)
// TODO: Forward-declare Policy class
// TODO: Define Node class
// TODO: Add fields: id, type, state, acceptedPayload, infectionPath
// TODO: Add adjacency vectors: providers, peers, customers
// TODO: Add policy field (unique_ptr<Policy>)
// TODO: Implement Node constructor, virtual destructor
// TODO: Implement receivePayload() and isHardened()

enum class NodeType{
    WORKSTATION,
    SERVER,
    DOMAIN_CONTROLLER,
    IOT_DEVICE,
    GATEWAY,
    FIREWALL
};

enum class infectionState{
    CLEAN,
    INFECTED,
    HARDENED
};

class Policy;

class Node{
    public:
        uint32_t id;
        NodeType type;
        infectionState state;
        std::string acceptredPayload;
        std::vector<uint32_t> infectionPath;
};
