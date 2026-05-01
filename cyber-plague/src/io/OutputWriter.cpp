#include "io/OutputWriter.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

// Writes one CSV row per infected node.
// infection_path is colon-separated (e.g. "1001:2045:3012") so the full path fits in a single CSV field.
void OutputWriter::writeResults(const std::vector<InfectionResult>& results,
                                 const std::string& outputPath) {
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }
    outFile << "node_id,accepted_payload,infection_path\n";
    for (const auto& result : results) {
        outFile << result.nodeId << "," << result.acceptedPayload << ",";

        // Join path IDs with ":" — index check avoids a trailing colon after the last entry
        std::ostringstream pathStream;
        for (size_t i = 0; i < result.infectionPath.size(); ++i) {
            pathStream << result.infectionPath[i];
            if (i < result.infectionPath.size() - 1) {
                pathStream << ":";
            }
        }
        outFile << pathStream.str() << "\n";
    }
    outFile.close();
}
