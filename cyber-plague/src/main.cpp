#include <iostream>
#include "simulation/Simulator.h"
#include "io/InputParser.h"
#include "io/OutputWriter.h"

int main(int argc, char* argv[]) {
    if(argc < 4){
        std::cerr << "Usage: " << argv[0] << " <patient_zero.csv> <hardened_nodes.csv> <edges.csv>" << std::endl;
        return 1;
    }

    InputParser parser;
    auto network = parser.buildNetwork(argv[1], argv[2], argv[3]);

    Simulator simulator(std::move(network));
    simulator.run();

    OutputWriter writer;
    writer.writeResults(simulator.getResults(), "infection_results.csv");

    return 0;
}
