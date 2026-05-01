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

namespace {
    constexpr const char* RESET  = "\033[0m";
    constexpr const char* BOLD   = "\033[1m";
    constexpr const char* GREEN  = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* CYAN   = "\033[36m";
    constexpr const char* WHITE  = "\033[37m";

    // Palette for payload types — skips green (hardened) and white (clean)
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

// Builds a stable payload->color mapping from whatever payloads exist in the network
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

void Simulator::drawMap(int wave, const std::set<uint32_t>& newlyInfected) const {
    std::cout << "\033[2J\033[H";

    if (wave == 0)
        std::cout << BOLD << CYAN << " CYBER GAME" << RESET << "  -  Initial State\n";
    else if (wave == -1)
        std::cout << BOLD << CYAN << " CYBER GAME" << RESET << "  -  " << BOLD << "Final State\n" << RESET;
    else
        std::cout << BOLD << CYAN << " CYBER GAME" << RESET << "  -  Wave " << BOLD << wave << RESET << "\n";

    std::cout << " " << std::string(60, '-') << "\n\n";

    const auto& nodes = network_->getNodes();
    auto payloadColors = buildPayloadColors(nodes);

    std::vector<uint32_t> ids;
    ids.reserve(nodes.size());
    for (const auto& [id, _] : nodes) ids.push_back(id);
    std::sort(ids.begin(), ids.end());

    const int COLS = 8;
    for (int i = 0; i < static_cast<int>(ids.size()); i++) {
        uint32_t id = ids[i];
        const Node* node = nodes.at(id).get();

        if (node->state == InfectionState::HARDENED) {
            std::cout << "  " << GREEN << std::setw(7) << std::left << id << RESET;
        } else if (node->state == InfectionState::INFECTED) {
            const char* color = payloadColors.count(node->acceptedPayload)
                                ? payloadColors.at(node->acceptedPayload)
                                : PALETTE[0];
            if (newlyInfected.count(id))
                std::cout << "  " << BOLD << color << std::setw(7) << std::left << id << RESET;
            else
                std::cout << "  " << color << std::setw(7) << std::left << id << RESET;
        } else {
            std::cout << "  " << WHITE << std::setw(7) << std::left << id << RESET;
        }

        if ((i + 1) % COLS == 0) std::cout << "\n";
    }
    if (ids.size() % COLS != 0) std::cout << "\n";

    // Stats
    int infected = 0, hardened = 0, clean = 0;
    for (uint32_t id : ids) {
        switch (nodes.at(id)->state) {
            case InfectionState::INFECTED: infected++; break;
            case InfectionState::HARDENED: hardened++; break;
            default:                       clean++;    break;
        }
    }

    std::cout << "\n " << std::string(60, '-') << "\n";
    std::cout << "  " << WHITE << "Clean: "    << clean    << RESET
              << "  "  << GREEN << "Hardened: " << hardened << RESET
              << "  Infected: " << infected << "\n";

    // Payload legend
    if (!payloadColors.empty()) {
        std::cout << "\n  Payloads:";
        for (const auto& [payload, color] : payloadColors)
            std::cout << "  " << color << payload << RESET;
        std::cout << "\n";
    }

    if (!newlyInfected.empty())
        std::cout << "  " << BOLD << "Bold" << RESET << " = newly infected this wave\n";
}

void Simulator::run() {
    network_->validateTopology();

    if (!animate_ && verbose_) {
        std::cout << BOLD << "Network loaded. " << RESET
                  << network_->size() << " nodes total.\n\n";
    } else if (!animate_) {
        std::cout << "Simulation has started... Total nodes: " << network_->size() << "\n";
    }

    propagate();

    int infected = 0, hardened = 0;
    for (const auto& [id, node] : network_->getNodes()) {
        if (node->state == InfectionState::INFECTED) {
            results_.push_back({node->id, node->acceptedPayload, node->infectionPath});
            infected++;
        } else if (node->state == InfectionState::HARDENED) {
            hardened++;
        }
    }

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

void Simulator::propagate() {
    std::queue<uint32_t> bfsQueue;
    for (const auto& [id, node] : network_->getNodes()) {
        if (node->state == InfectionState::INFECTED)
            bfsQueue.push(id);
    }

    if (animate_) drawMap(0, {});

    int wave = 0;
    while (!bfsQueue.empty()) {
        wave++;
        int waveSize = static_cast<int>(bfsQueue.size());
        std::set<uint32_t> newlyInfected;

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
                    neighborNode->infectionPath = currentNode->infectionPath;
                    neighborNode->infectionPath.push_back(neighborNode->id);
                    bfsQueue.push(neighborId);
                    newlyInfected.insert(neighborId);
                    if (verbose_ && !animate_)
                        std::cout << "[INFECTED]     (" << edgeType << ")\n";
                }
            };

            for (uint32_t neighborId : currentNode->providers) tryInfect(neighborId, "provider");
            for (uint32_t neighborId : currentNode->peers)     tryInfect(neighborId, "peer");
            for (uint32_t neighborId : currentNode->customers) tryInfect(neighborId, "customer");
        }

        if (animate_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
            drawMap(wave, newlyInfected);
        }
    }
}
