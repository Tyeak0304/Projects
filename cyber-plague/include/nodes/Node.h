#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

enum class NodeType {
    WORKSTATION,
    SERVER,
    DOMAIN_CONTROLLER,
    IOT_DEVICE,
    GATEWAY,
    FIREWALL
};

enum class InfectionState {
    CLEAN,
    INFECTED,
    HARDENED  // IDS/Firewall active
};

class Policy;

class Node {
public:
    uint32_t id;
    NodeType type;
    InfectionState state;
    std::string acceptedPayload;
    std::vector<uint32_t> infectionPath;

    // Propagation Rank adjacency: up / across / down
    std::vector<uint32_t> providers;   // up   (Domain Controllers, Servers)
    std::vector<uint32_t> peers;       // across (Workstations)
    std::vector<uint32_t> customers;   // down  (IoT, managed devices)

    std::unique_ptr<Policy> policy;

    explicit Node(uint32_t id, NodeType type);
    virtual ~Node() = default;

    bool receivePayload(const std::string& payload, uint32_t sourceId);
    bool isHardened() const { return state == InfectionState::HARDENED; }
};
