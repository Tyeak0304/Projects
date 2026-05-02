#include "io/OutputWriter.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

// Write the simulation results to a CSV file — one row per infected node.
// The infection path is stored as node IDs joined by colons (e.g. "1001:2045:3012")
// so the entire chain fits neatly inside a single CSV column.
void OutputWriter::writeResults(const std::vector<InfectionResult>& results,
                                 const std::string& outputPath) {
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }

    // Write the header row first so the CSV columns have names.
    outFile << "node_id,accepted_payload,infection_path\n";

    for (const auto& result : results) {
        outFile << result.nodeId << "," << result.acceptedPayload << ",";

        // Build the colon-separated path string.
        // We check the index to avoid adding a trailing colon after the last ID.
        std::ostringstream pathStream;
        for (size_t i = 0; i < result.infectionPath.size(); ++i) {
            pathStream << result.infectionPath[i];
            if (i < result.infectionPath.size() - 1) {
                pathStream << ":"; // separator between IDs, but not after the last one
            }
        }
        outFile << pathStream.str() << "\n";
    }

    outFile.close();
}
