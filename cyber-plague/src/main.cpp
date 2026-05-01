#include <iostream>
#include <string>
#include "simulation/Simulator.h"
#include "io/InputParser.h"
#include "io/OutputWriter.h"

#ifdef _WIN32
#include <windows.h>
static void enableAnsi() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enableAnsi() {}
#endif

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool animate = true;   // on by default; --verbose/-v disables it
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") { verbose = true; animate = false; }
        if (arg == "--no-animate")              { animate = false; }
    }

    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <patient_zero.csv> <hardened_nodes.csv> <edges.csv> [--verbose|-v] [--animate|-a]\n";
        return 1;
    }

    if (verbose || animate) enableAnsi();

    InputParser parser;
    auto network = parser.buildNetwork(argv[1], argv[2], argv[3]);

    Simulator simulator(std::move(network), verbose, animate);
    simulator.run();

    OutputWriter writer;
    writer.writeResults(simulator.getResults(), "infection_results.csv");
    if (!animate)
        std::cout << "Wrote " << simulator.getResults().size() << " infected nodes to infection_results.csv\n";

    return 0;
}
