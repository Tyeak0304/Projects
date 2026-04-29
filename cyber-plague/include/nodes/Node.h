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

enum class InfectionState{
    CLEAN,
    INFECTED,
    HARDENED
};

class Policy;

class Node{
    public:
        //Node fields
        uint32_t id;
        NodeType type;
        InfectionState state;
        std::string acceptedPayload;
        std::unique_ptr<Policy> policy;

        //Adjacency lists
        std::vector<uint32_t> infectionPath;
        std::vector<Node*> providers;
        std::vector<Node*> peers;
        std::vector<Node*> customers;
        
        //Node Constructor and virtual destructor
        Node(uint32_t id, NodeType type) : id(id), type(type), state(InfectionState::CLEAN) {}
        virtual ~Node();

        //receivePayload and isHardened functions
        virtual bool receivePayload(const std::string& payload, uint32_t sourceId) = 0;
        
        bool isHardened() const;


};
