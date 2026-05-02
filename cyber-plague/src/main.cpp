#include <iostream>
#include <string>
#include "simulation/Simulator.h"
#include "io/InputParser.h"
#include "io/OutputWriter.h"

// Windows terminals don't support colored text by default.
// This block enables ANSI color codes (the escape sequences used for colors)
// on Windows. On Linux and macOS, colors work out of the box so this does nothing.
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
    // Default behavior: show the animated map. --verbose or --no-animate turns it off.
    bool verbose = false;
    bool animate = true;

    // Check each command-line argument for known flags.
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") { verbose = true; animate = false; }
        if (arg == "--no-animate")              { animate = false; }
    }

    // The program requires exactly three CSV file paths as arguments.
    // Print a usage message and exit if they weren't provided.
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <patient_zero.csv> <hardened_nodes.csv> <edges.csv> [--verbose|-v] [--no-animate]\n";
        return 1;
    }

    // Enable colored terminal output if we're going to print anything with color.
    if (verbose || animate) enableAnsi();

    // Read the three CSV files and build the network of machines and connections.
    InputParser parser;
    auto network = parser.buildNetwork(argv[1], argv[2], argv[3]);

    // Run the simulation — spread the infection across the network.
    Simulator simulator(std::move(network), verbose, animate);
    simulator.run();

    // Save the results to a CSV file in the Results folder.
    OutputWriter writer;
    writer.writeResults(simulator.getResults(), "Results/infection_results.csv");

    // If animation is off, print a short summary to the terminal instead.
    if (!animate)
        std::cout << "Wrote " << simulator.getResults().size() << " infected nodes to infection_results.csv\n";

    return 0;
}
