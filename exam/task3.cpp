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
#include <matplot/matplot.h>

int main()
{
    using namespace matplot;

    std::vector<std::string> species = {
        "Diptera", "Hymenoptera", "Hemiptera", "Psocoptera", "Coleoptera",
        "Collembola", "Arachnid", "Thysanura", "Isoptera", "Lepidoptera",
        "Neuroptera", "Larave", "Orthoptera", "Unident"};

    std::unordered_map<std::string, std::unordered_map<std::string, int>> counts;
    std::unordered_map<std::string, double> speciesTotals;
    
    std::unordered_map<std::string, std::unordered_map<std::string, double>> lightSpeciesCounts;
    std::set<std::string> lightTypes;

    std::ifstream file("bug-attraction.csv");
    if (!file.is_open())
    {
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

    while (std::getline(headerStream, header, delimiter))
    {
        headers.push_back(header);
    }

    // indices of required columns ( re-used across tasks )
    int lightTypeIndex = -1;
    std::unordered_map<std::string, int> speciesIndices;
    for (size_t i = 0; i < headers.size(); ++i)
    {
        if (headers[i] == "Light Type")
        {
            lightTypeIndex = i;
        }
        for (const auto &sp : species)
        {
            if (headers[i] == sp)
            {
                speciesIndices[sp] = i;
            }
        }
    }

    // read data
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, delimiter))
        {
            row.push_back(cell);
        }

        if (row.size() != headers.size())
        {
            continue;
        }

        std::string lightType = row[lightTypeIndex];
        lightTypes.insert(lightType);

        for (const auto &sp : species)
        {
            int index = speciesIndices[sp];
            int count = 0;
            try
            {
                count = std::stoi(row[index]);
            }
            catch (...)
            {
                count = 0;
            }
            counts[sp][lightType] += count;

            speciesTotals[sp] += count;
            lightSpeciesCounts[lightType][sp] += count;
        }
    }


    // some debug....
    // for (const auto &sp : counts) {
    //     std::cout << sp.first << std::endl;

    //     for (const auto &count : sp.second) {
    //         std::cout << count.first << " : " << count.second << std::endl;
    //     }
    // }

    std::vector<std::pair<std::string, double>> speciesVector(speciesTotals.begin(), speciesTotals.end());
    std::sort(speciesVector.begin(), speciesVector.end(),
        [](const auto &a, const auto &b)
        { return a.second > b.second; });

    std::vector<std::string> topSpecies;
    for (size_t i = 0; i < 4 && i < speciesVector.size(); ++i)
    {
        topSpecies.push_back(speciesVector[i].first);
    }

    std::vector<std::string> lightTypeVector(lightTypes.begin(), lightTypes.end());
    size_t num_species = topSpecies.size();
    size_t num_light_types = lightTypeVector.size();

    std::vector<std::vector<double>> data(num_species, std::vector<double>(num_light_types, 0.0));

    for (size_t i = 0; i < num_species; ++i)
    {
        const auto &sp = topSpecies[i];
        for (size_t j = 0; j < num_light_types; ++j)
        {
            const auto &lt = lightTypeVector[j];
            data[i][j] = lightSpeciesCounts[lt][sp];
        }
    }

    std::vector<double> positions(num_light_types);
    std::iota(positions.begin(), positions.end(), 1);

    double total_bar_width = 0.8;
    double bar_width = total_bar_width / num_species;

    auto ax = gca();

    ax->hold(true);

    for (size_t i = 0; i < num_species; ++i)
    {
        std::vector<double> y_values(num_light_types);
        for (size_t j = 0; j < num_light_types; ++j)
        {
            y_values[j] = data[i][j];
        }

        // shift x positions for each species
        std::vector<double> x_positions(num_light_types);
        for (size_t j = 0; j < num_light_types; ++j)
        {
            // I spent months building a responsive candlestick chart a few years ago, this class would have helped...
            // I drew everything with svgs and it got to a scaling point where I should have used a buffer vector because it got so slow
            x_positions[j] = positions[j] - total_bar_width / 2 + (i + 0.5) * bar_width;
        }

        // bars for this species
        auto b = bar(x_positions, y_values);
        b->bar_width(bar_width);
        b->display_name(topSpecies[i]);
    }

    // x-ticks and labels
    ax->xticks(positions);
    ax->xticklabels(lightTypeVector);

    // ambigious legend issue
    ::matplot::legend();

    // labels and title
    xlabel("Light Type");
    ylabel("Total Count");
    title("Total Counts of Top 4 Species per Light Type");

    show();

    return 0;
}