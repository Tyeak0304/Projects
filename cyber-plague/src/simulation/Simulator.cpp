#include "simulation/Simulator.h"
#include <queue>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

// ANSI escape codes for terminal colors.
// These are special character sequences that tell the terminal to change text color.
namespace {
    constexpr const char* RESET  = "\033[0m";   // go back to the default color
    constexpr const char* BOLD   = "\033[1m";   // make text bold
    constexpr const char* GREEN  = "\033[32m";  // used for hardened (protected) nodes
    constexpr const char* YELLOW = "\033[33m";  // used for warning-style messages
    constexpr const char* CYAN   = "\033[36m";  // used for the title and wave headers
    constexpr const char* WHITE  = "\033[37m";  // used for clean (uninfected) nodes

    // A set of colors used to visually distinguish different malware payloads on the map.
    // Green and white are skipped here because those are reserved for hardened and clean nodes.
    const char* PALETTE[] = {
        "\033[31m",   // red
        "\033[35m",   // magenta
        "\033[33m",   // yellow
        "\033[94m",   // bright blue
        "\033[96m",   // bright cyan
        "\033[91m",   // bright red
        "\033[95m",   // bright magenta
        "\033[93m",   // bright yellow
    };
    constexpr int PALETTE_SIZE = 8;
}

// Assign a unique color to each distinct malware payload found in the network.
// This is built once before drawing so the same payload always appears in the same color.
static std::map<std::string, const char*> buildPayloadColors(
    const std::unordered_map<uint32_t, std::unique_ptr<Node>>& nodes)
{
    std::map<std::string, const char*> colors;
    int idx = 0;
    for (const auto& [id, node] : nodes) {
        if (node->state == InfectionState::INFECTED && !node->acceptedPayload.empty()) {
            if (!colors.count(node->acceptedPayload))
                colors[node->acceptedPayload] = PALETTE[idx++ % PALETTE_SIZE];
        }
    }
    return colors;
}

// Draw the full network map to the terminal, color-coded by infection state.
// Newly infected nodes this wave are shown in bold so they stand out.
// wave == 0 means the initial state; wave == -1 means the final state.
void Simulator::drawMap(int wave, const std::set<uint32_t>& newlyInfected) const {
    // Clear the screen and move the cursor to the top-left corner.
    std::cout << "\033[2J\033[H";

    // Print the header showing which wave we're on.
    if (wave == 0)
        std::cout << BOLD << CYAN << " CYBER GAME" << RESET << "  -  Initial State\n";
    else if (wave == -1)
        std::cout << BOLD << CYAN << " CYBER GAME" << RESET << "  -  " << BOLD << "Final State\n" << RESET;
    else
        std::cout << BOLD << CYAN << " CYBER GAME" << RESET << "  -  Wave " << BOLD << wave << RESET << "\n";

    std::cout << " " << std::string(60, '-') << "\n\n";

    const auto& nodes = network_->getNodes();
    auto payloadColors = buildPayloadColors(nodes);

    // Collect all node IDs and sort them so they display in a consistent order.
    std::vector<uint32_t> ids;
    ids.reserve(nodes.size());
    for (const auto& [id, _] : nodes) ids.push_back(id);
    std::sort(ids.begin(), ids.end());

    // Print nodes in a grid, 8 per row, each colored by their infection state.
    const int COLS = 8;
    for (int i = 0; i < static_cast<int>(ids.size()); i++) {
        uint32_t id = ids[i];
        const Node* node = nodes.at(id).get();

        if (node->state == InfectionState::HARDENED) {
            // Hardened nodes are always green.
            std::cout << "  " << GREEN << std::setw(7) << std::left << id << RESET;
        } else if (node->state == InfectionState::INFECTED) {
            // Infected nodes use the color assigned to their payload.
            // Nodes infected this wave are shown in bold to highlight the spread.
            const char* color = payloadColors.count(node->acceptedPayload)
                                ? payloadColors.at(node->acceptedPayload)
                                : PALETTE[0];
            if (newlyInfected.count(id))
                std::cout << "  " << BOLD << color << std::setw(7) << std::left << id << RESET;
            else
                std::cout << "  " << color << std::setw(7) << std::left << id << RESET;
        } else {
            // Clean nodes are white.
            std::cout << "  " << WHITE << std::setw(7) << std::left << id << RESET;
        }

        if ((i + 1) % COLS == 0) std::cout << "\n"; // start a new row every 8 nodes
    }
    if (ids.size() % COLS != 0) std::cout << "\n"; // end the last row if it wasn't a full 8

    // Count up totals for the summary line at the bottom.
    int infected = 0, hardened = 0, clean = 0;
    for (uint32_t id : ids) {
        switch (nodes.at(id)->state) {
            case InfectionState::INFECTED: infected++; break;
            case InfectionState::HARDENED: hardened++; break;
            default:                       clean++;    break;
        }
    }

    // Print the stats bar and payload color legend.
    std::cout << "\n " << std::string(60, '-') << "\n";
    std::cout << "  " << WHITE << "Clean: "    << clean    << RESET
              << "  "  << GREEN << "Hardened: " << hardened << RESET
              << "  Infected: " << infected << "\n";

    if (!payloadColors.empty()) {
        std::cout << "\n  Payloads:";
        for (const auto& [payload, color] : payloadColors)
            std::cout << "  " << color << payload << RESET;
        std::cout << "\n";
    }

    if (!newlyInfected.empty())
        std::cout << "  " << BOLD << "Bold" << RESET << " = newly infected this wave\n";
}

// Entry point for the simulation. Steps:
//   1. Validate the network (check for circular connections).
//   2. Run the BFS propagation (spread the infection wave by wave).
//   3. Collect the final results — which nodes got infected and how.
void Simulator::run() {
    network_->validateTopology();

    if (!animate_ && verbose_) {
        std::cout << BOLD << "Network loaded. " << RESET
                  << network_->size() << " nodes total.\n\n";
    } else if (!animate_) {
        std::cout << "Simulation has started... Total nodes: " << network_->size() << "\n";
    }

    propagate();

    // After propagation is done, collect results for every infected node.
    int infected = 0, hardened = 0;
    for (const auto& [id, node] : network_->getNodes()) {
        if (node->state == InfectionState::INFECTED) {
            results_.push_back({node->id, node->acceptedPayload, node->infectionPath});
            infected++;
        } else if (node->state == InfectionState::HARDENED) {
            hardened++;
        }
    }

    // Show the final state of the map and print a summary.
    if (animate_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        drawMap(-1, {});
        std::cout << "\n  " << BOLD << "Simulation complete. " << RESET
                  << infected << " infected, "
                  << GREEN << hardened << " blocked" << RESET << ".\n\n";
    } else if (verbose_) {
        std::cout << "\n" << BOLD << "Simulation complete. " << RESET
                  << infected << " infected, "
                  << GREEN << hardened << " blocked" << RESET << ".\n";
    }
}

const std::vector<InfectionResult>& Simulator::getResults() const {
    return results_;
}

// The core of the simulation — spreads infection wave by wave using BFS.
// BFS (breadth-first search) means we infect all nodes reachable in one hop
// before moving on to the next hop, so each "wave" represents one step of spread.
//
// How it works:
//   - Start by putting all patient zero nodes into a queue.
//   - Each iteration of the outer loop is one wave.
//   - For each node in the current wave, try to infect its neighbors.
//   - Newly infected neighbors go into the queue to spread in the next wave.
//   - Stop when the queue is empty (no more nodes to infect).
void Simulator::propagate() {
    std::queue<uint32_t> bfsQueue;

    // Seed the queue with every node that starts out infected (the patient zeros).
    for (const auto& [id, node] : network_->getNodes()) {
        if (node->state == InfectionState::INFECTED)
            bfsQueue.push(id);
    }

    if (animate_) drawMap(0, {}); // show the initial state before any spreading

    int wave = 0;
    while (!bfsQueue.empty()) {
        wave++;
        // Only process the nodes that were in the queue at the start of this wave.
        // Nodes added during this wave belong to the next wave.
        int waveSize = static_cast<int>(bfsQueue.size());
        std::set<uint32_t> newlyInfected; // track which nodes were infected this wave for the map display

        while (waveSize-- > 0) {
            uint32_t current = bfsQueue.front();
            bfsQueue.pop();
            Node* currentNode = network_->getNode(current);
            if (currentNode == nullptr || currentNode->state != InfectionState::INFECTED) continue;

            if (verbose_ && !animate_) {
                std::cout << BOLD << CYAN << "[WAVE " << wave << "] " << RESET
                          << "Node " << current << " spreading "
                          << currentNode->acceptedPayload << "...\n";
            }

            // Try to infect one neighbor. If successful, record the path and add it to the queue.
            auto tryInfect = [&](uint32_t neighborId, const std::string& edgeType) {
                Node* neighborNode = network_->getNode(neighborId);
                if (neighborNode == nullptr) return;

                if (verbose_ && !animate_)
                    std::cout << "  -> " << std::setw(6) << neighborId << "  ";

                if (neighborNode->state == InfectionState::HARDENED) {
                    if (verbose_ && !animate_)
                        std::cout << GREEN << "[BLOCKED]      " << RESET << "(" << edgeType << ", hardened)\n";
                    return;
                }
                if (neighborNode->state == InfectionState::INFECTED) {
                    if (verbose_ && !animate_)
                        std::cout << YELLOW << "[ALREADY INF]  " << RESET << "(" << edgeType << ")\n";
                    return;
                }
                if (neighborNode->receivePayload(currentNode->acceptedPayload)) {
                    // Carry the infection path forward: copy the current path and append this new node.
                    neighborNode->infectionPath = currentNode->infectionPath;
                    neighborNode->infectionPath.push_back(neighborNode->id);
                    bfsQueue.push(neighborId);
                    newlyInfected.insert(neighborId);
                    if (verbose_ && !animate_)
                        std::cout << "[INFECTED]     (" << edgeType << ")\n";
                }
            };

            // Spread in hierarchy order: providers (up) → peers (across) → customers (down).
            for (uint32_t neighborId : currentNode->providers) tryInfect(neighborId, "provider");
            for (uint32_t neighborId : currentNode->peers)     tryInfect(neighborId, "peer");
            for (uint32_t neighborId : currentNode->customers) tryInfect(neighborId, "customer");
        }

        // Pause briefly between waves so the animation is visible to the human eye.
        if (animate_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
            drawMap(wave, newlyInfected);
        }
    }
}
