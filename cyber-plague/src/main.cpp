#include <iostream>
#include "simulation/Simulator.h"
#include "io/InputParser.h"
#include "io/OutputWriter.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <patient_zero.csv> <hardened_nodes.csv>\n";
        return 1;
    }

    InputParser parser;
    auto network = parser.buildNetwork(argv[1], argv[2]);

    Simulator sim(std::move(network));
    sim.run();

    OutputWriter writer;
    writer.writeResults(sim.getResults(), "data/output/infection_results.csv");

    return 0;
}
