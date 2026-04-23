#include "io/OutputWriter.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

// TODO: Implement OutputWriter::writeResults
//   Purpose: write a CSV documenting the final infection state of the network
//
//   Output format (one row per infected node):
//     node_id,accepted_payload,infection_path
//   Where infection_path is a colon-separated list of node IDs, e.g.:
//     1001,ransomware_v2,1001:2045:3012
//
//   Steps:
//   1. Open outputPath with std::ofstream, throw std::runtime_error if it fails
//   2. Write the header line: "node_id,accepted_payload,infection_path\n"
//   3. For each InfectionResult in results:
//        a. Write result.nodeId
//        b. Write "," then result.acceptedPayload
//        c. Build the path string by iterating result.infectionPath
//           Join with ":" separator (no trailing colon)
//           Hint: use a loop with index check, or std::ostringstream
//        d. Write "," then the path string, then "\n"
//   4. Print a summary to std::cout:
//        "Wrote <N> infected nodes to <outputPath>\n"
void OutputWriter::writeResults(const std::vector<InfectionResult>& results,
                                 const std::string& outputPath) {
    // TODO: Fill in writeResults body
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }
    outFile << "node_id,accepted_payload,infection_path\n";
    for (const auto& result : results) {
        outFile << result.nodeId << "," << result.acceptedPayload << ",";
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
    std::cout << "Wrote " << results.size() << " infected nodes to " << outputPath << "\n";
}
