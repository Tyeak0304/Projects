#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

// What kind of machine this node represents in the simulated network.
// Each type is here for realism, but they all behave the same way in the simulation.
enum class NodeType {
    WORKSTATION,        // a regular user's computer
    SERVER,             // a machine that provides services to other computers
    DOMAIN_CONTROLLER,  // a special server that manages user accounts and permissions
    IOT_DEVICE,         // a small internet-connected device like a printer or thermostat
    GATEWAY,            // a machine that bridges two separate networks
    FIREWALL            // a machine whose only job is to block unwanted traffic
};

// Tracks whether a node is healthy, infected, or protected.
enum class InfectionState {
    CLEAN,    // the node has not been infected
    INFECTED, // the node is carrying a malware payload
    HARDENED  // the node is protected and cannot be infected
};

class Policy;

// Represents one machine in the simulated network.
// Each node knows its neighbors, its current infection state,
// and the rules it uses to decide whether to accept incoming malware.
class Node {
public:
    uint32_t id;                         // unique number that identifies this machine
    NodeType type;                       // what kind of machine this is
    InfectionState state;                // whether this machine is healthy, infected, or protected
    std::string acceptedPayload;         // name of the malware infecting this node (empty if clean)
    std::unique_ptr<Policy> policy;      // the rules this node uses to accept or reject malware

    std::vector<uint32_t> infectionPath; // the chain of node IDs the malware traveled through to reach here
    std::vector<uint32_t> providers;     // neighbors that are "above" this node (e.g. servers this machine connects to)
    std::vector<uint32_t> peers;         // neighbors at the same level (e.g. other workstations)
    std::vector<uint32_t> customers;     // neighbors that are "below" this node (e.g. IoT devices this machine serves)

    Node(uint32_t id, NodeType type);
    virtual ~Node();

    // Try to infect this node with the given malware payload.
    // Returns true if the node became infected, false if it was blocked or already infected.
    virtual bool receivePayload(const std::string& payload);

    // Returns true if this node is protected and immune to infection.
    bool isHardened() const;
};
