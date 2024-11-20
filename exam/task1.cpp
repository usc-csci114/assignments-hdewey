#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>

int main() {

    // species list
    std::vector<std::string> species = {
        "Diptera", "Hymenoptera", "Hemiptera", "Psocoptera", "Coleoptera",
        "Collembola", "Arachnid", "Thysanura", "Isoptera", "Lepidoptera",
        "Neuroptera", "Larave", "Orthoptera", "Unident"
    };

    // Map to hold counts: species -> light type -> count
    std::unordered_map<std::string, std::unordered_map<std::string, int>> counts;

    std::set<std::string> lightTypes;

    // open csv
    std::ifstream file("bug-attraction.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open data.csv" << std::endl;
        return 1;
    }

    // header line
    std::string line;
    std::getline(file, line);
    std::vector<std::string> headers;
    std::stringstream headerStream(line);
    std::string header;
    char delimiter = ',';

    while (std::getline(headerStream, header, delimiter)) {
        headers.push_back(header);
    }

    // indices of required columns ( re-used across tasks )
    int lightTypeIndex = -1;
    int moonIndex = -1;
    int totalIndex = -1;
    std::unordered_map<std::string, int> speciesIndices;
    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] == "Light Type") {
            lightTypeIndex = i;
        } else if (headers[i] == "Standardized Moon") {
            moonIndex = i;
        } else if (headers[i] == "Total") {
            totalIndex = i;
        }
        for (const auto& sp : species) {
            if (headers[i] == sp) {
                speciesIndices[sp] = i;
            }
        }
    }

    // read data
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, delimiter)) {
            row.push_back(cell);
        }

        if (row.size() != headers.size()) {
            continue;
        }

        std::string lightType = row[lightTypeIndex];
        lightTypes.insert(lightType);

        for (const auto& sp : species) {
            int index = speciesIndices[sp];
            int count = 0;
            try {
                count = std::stoi(row[index]);
            } catch (...) {
                count = 0;
            }
            counts[sp][lightType] += count;
        }
    }

    std::cout << std::left << std::setw(15) << "Species" << "Most Attractive Light Type\n";
    std::cout << "-------------------------------------------\n";
    for (const auto& sp : species) {
        const auto& lightCounts = counts[sp];
        std::string maxLight;
        int maxCount = 0;
        for (const auto& lc : lightCounts) {
            if (lc.second > maxCount) {
                maxCount = lc.second;
                maxLight = lc.first;
            }
        }
        std::cout << std::left << std::setw(15) << sp << maxLight << '\n';
    }

    return 0;
}