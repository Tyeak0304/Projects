#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

// Models the role of each machine in the simulated network
enum class NodeType{
    WORKSTATION,
    SERVER,
    DOMAIN_CONTROLLER,
    IOT_DEVICE,
    GATEWAY,
    FIREWALL
};

// Tracks whether a node is clean, actively infected, or hardened (immune)
enum class InfectionState{
    CLEAN,
    INFECTED,
    HARDENED
};

class Policy;

class Node{
    public:
        uint32_t id;
        NodeType type;
        InfectionState state;
        std::string acceptedPayload;         // name of the payload currently infecting this node
        std::unique_ptr<Policy> policy;      // controls how this node filters and accepts payloads

        std::vector<uint32_t> infectionPath; // ordered list of node IDs the infection traveled through to reach here
        std::vector<uint32_t> providers;     // upstream neighbors (higher-tier relationships)
        std::vector<uint32_t> peers;         // lateral neighbors (same-tier relationships)
        std::vector<uint32_t> customers;     // downstream neighbors (lower-tier relationships)

        Node(uint32_t id, NodeType type);
        virtual ~Node();

        // Attempts to infect this node with the given payload from sourceId.
        // Returns true if the node became newly infected, false if blocked or already infected.
        virtual bool receivePayload(const std::string& payload, uint32_t sourceId);

        // Returns true if this node's state is HARDENED (immune to infection)
        bool isHardened() const;
};
