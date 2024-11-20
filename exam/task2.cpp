#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <matplot/matplot.h>

void linearRegression(const std::vector<double> &x, const std::vector<double> &y, double &slope, double &intercept, double &r2)
{
    size_t n = x.size();

    if (n == 0)
    {
        slope = 0;
        intercept = 0;
        r2 = 0;
        return;
    }

    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumX2 = 0.0;

    for (size_t i = 0; i < n; ++i)
    {
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }

    double numerator = n * sumXY - sumX * sumY;
    double denominator = n * sumX2 - sumX * sumX;

    if (denominator == 0)
    {
        slope = 0;
        intercept = sumY / n; // meanY
        r2 = 0;
        return;
    }

    slope = numerator / denominator;
    intercept = (sumY - slope * sumX) / n;

    double ssTot = 0.0;
    double ssRes = 0.0;
    double meanY = sumY / n;

    for (size_t i = 0; i < n; ++i)
    {
        double yPred = slope * x[i] + intercept;
        ssRes += std::pow(y[i] - yPred, 2);
        ssTot += std::pow(y[i] - meanY, 2);
    }

    r2 = (ssTot != 0) ? 1 - (ssRes / ssTot) : 0;
}

int main()
{
    using namespace matplot;

    // vec for BG site
    std::vector<double> standardizedMoon_BG;
    std::vector<double> totalBugs_BG;

    // vec for LK and Stunt sites
    std::vector<double> standardizedMoon_LK_Stunt;
    std::vector<double> totalBugs_LK_Stunt;

    // open csv
    std::ifstream file("bug-attraction.csv");
    if (!file.is_open())
    {
        std::cerr << "Failed to open bug-attraction.csv" << std::endl;
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
    int moonIndex = -1;
    int totalIndex = -1;
    int siteIndex = -1;

    for (size_t i = 0; i < headers.size(); ++i)
    {
        if (headers[i] == "Standardized Moon")
        {
            moonIndex = i;
        }
        else if (headers[i] == "Total")
        {
            totalIndex = i;
        }
        else if (headers[i] == "Location")
        {
            siteIndex = i;
        }
    }

    // are all required indices found?
    if (moonIndex == -1 || totalIndex == -1 || siteIndex == -1)
    {
        std::cerr << "Required columns are missing in the CSV file." << std::endl;
        return 1;
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

        // site nameeee
        std::string site = row[siteIndex];

        try
        {
            double moonValue = std::stod(row[moonIndex]);
            double totalValue = std::stod(row[totalIndex]);

            if (site == "BG")
            {
                standardizedMoon_BG.push_back(moonValue);
                totalBugs_BG.push_back(totalValue);
            }
            else if (site == "LK" || site == "Stunt")
            {
                standardizedMoon_LK_Stunt.push_back(moonValue);
                totalBugs_LK_Stunt.push_back(totalValue);
            }
        }
        catch (...)
        {
            continue;
        }
    }

    // lin reg for BG site
    double slope_BG = 0, intercept_BG = 0, r2_BG = 0;
    if (!standardizedMoon_BG.empty())
    {
        linearRegression(standardizedMoon_BG, totalBugs_BG, slope_BG, intercept_BG, r2_BG);
    }

    // lin reg for LK and Stunt sites
    double slope_LK_Stunt = 0, intercept_LK_Stunt = 0, r2_LK_Stunt = 0;
    if (!standardizedMoon_LK_Stunt.empty())
    {
        linearRegression(standardizedMoon_LK_Stunt, totalBugs_LK_Stunt, slope_LK_Stunt, intercept_LK_Stunt, r2_LK_Stunt);
    }

    // tiled plot layout
    auto f = figure(true);
    tiledlayout(2, 1);

    // subplot for BG
    if (!standardizedMoon_BG.empty())
    {
        auto ax1 = nexttile();
        scatter(ax1, standardizedMoon_BG, totalBugs_BG)->marker_face_color("blue").marker_size(6);
        hold(ax1, on);

        // regression line for BG site
        std::vector<double> regX_BG = {
            *std::min_element(standardizedMoon_BG.begin(), standardizedMoon_BG.end()),
            *std::max_element(standardizedMoon_BG.begin(), standardizedMoon_BG.end())};
        std::vector<double> regY_BG = {
            slope_BG * regX_BG[0] + intercept_BG,
            slope_BG * regX_BG[1] + intercept_BG};
        plot(ax1, regX_BG, regY_BG, "r-");

        // annotate the plot for BG site
        xlabel(ax1, "Standardized Moon");
        ylabel(ax1, "Total Number of Bugs");
        title(ax1, "BG Site");

        // regression equation and R^2 value for BG site
        std::ostringstream ss_BG;
        ss_BG << std::fixed << std::setprecision(2);
        ss_BG << "y = " << slope_BG << "x + " << intercept_BG << "\nR^2 = " << r2_BG;

        // position the text box for BG site
        double x_text_BG = *std::min_element(standardizedMoon_BG.begin(), standardizedMoon_BG.end()) +
            (*std::max_element(standardizedMoon_BG.begin(), standardizedMoon_BG.end()) -
                *std::min_element(standardizedMoon_BG.begin(), standardizedMoon_BG.end())) * 0.05;
        
        double y_text_BG = *std::max_element(totalBugs_BG.begin(), totalBugs_BG.end()) -
            (*std::max_element(totalBugs_BG.begin(), totalBugs_BG.end()) -
                *std::min_element(totalBugs_BG.begin(), totalBugs_BG.end())) * 0.05;
        
        text(ax1, x_text_BG, y_text_BG, ss_BG.str());
    }
    else
    {
        std::cerr << "No data available for BG site." << std::endl;
    }

    // subplot for LK and Stunt sites
    if (!standardizedMoon_LK_Stunt.empty())
    {
        auto ax2 = nexttile();
        scatter(ax2, standardizedMoon_LK_Stunt, totalBugs_LK_Stunt)->marker_face_color("green").marker_size(6);
        hold(ax2, on);

        // regression line for LK and Stunt sites
        std::vector<double> regX_LK_Stunt = {
            *std::min_element(standardizedMoon_LK_Stunt.begin(), standardizedMoon_LK_Stunt.end()),
            *std::max_element(standardizedMoon_LK_Stunt.begin(), standardizedMoon_LK_Stunt.end())};
        std::vector<double> regY_LK_Stunt = {
            slope_LK_Stunt * regX_LK_Stunt[0] + intercept_LK_Stunt,
            slope_LK_Stunt * regX_LK_Stunt[1] + intercept_LK_Stunt};
        plot(ax2, regX_LK_Stunt, regY_LK_Stunt, "r-");

        // annotate the plot for LK and Stunt sites
        xlabel(ax2, "Standardized Moon");
        ylabel(ax2, "Total Number of Bugs");
        title(ax2, "LK and Stunt Sites");

        // regression equation and R^2 value for LK and Stunt sites
        std::ostringstream ss_LK_Stunt;
        ss_LK_Stunt << std::fixed << std::setprecision(2);
        ss_LK_Stunt << "y = " << slope_LK_Stunt << "x + " << intercept_LK_Stunt << "\nR^2 = " << r2_LK_Stunt;

        // position the text box for LK and Stunt sites
        double x_text_LK_Stunt = *std::min_element(standardizedMoon_LK_Stunt.begin(), standardizedMoon_LK_Stunt.end()) +
            (*std::max_element(standardizedMoon_LK_Stunt.begin(), standardizedMoon_LK_Stunt.end()) -
                *std::min_element(standardizedMoon_LK_Stunt.begin(), standardizedMoon_LK_Stunt.end())) * 0.05;
        double y_text_LK_Stunt = *std::max_element(totalBugs_LK_Stunt.begin(), totalBugs_LK_Stunt.end()) -
            (*std::max_element(totalBugs_LK_Stunt.begin(), totalBugs_LK_Stunt.end()) -
                *std::min_element(totalBugs_LK_Stunt.begin(), totalBugs_LK_Stunt.end())) * 0.05;
        
        text(ax2, x_text_LK_Stunt, y_text_LK_Stunt, ss_LK_Stunt.str());
    }
    else
    {
        std::cerr << "No data available for LK and Stunt sites." << std::endl;
    }

    // show them alll
    show();

    return 0;
}