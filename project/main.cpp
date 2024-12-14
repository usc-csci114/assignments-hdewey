#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <numeric>
#include <cmath>

#include <matplot/matplot.h>

namespace plt = matplot;

// struct to hold student data
struct Student
{
    std::string roll_no;
    std::string gender;
    std::string race_ethnicity;
    std::string parental_level_of_education;
    int lunch;                   // 1: free/reduced, 0: not
    int test_preparation_course; // 1: completed, 0: not
    int math_score;
    int science_score;
    int reading_score;
    int writing_score;
    int total_score;
    char grade;
};

// func to trim whitespace
std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first)
    {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// func to parse a CSV line into a Student object
bool parse_line(const std::string &line, Student &student)
{
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;

    while (std::getline(ss, item, ','))
    {
        tokens.push_back(trim(item));
    }

    if (tokens.size() != 12)
    {
        // debug
        // std::cerr << "Invalid line (incorrect number of fields): " << line << std::endl;
        return false;
    }

    // Check for empty fields
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i].empty())
        {
            // debug
            // std::cerr << "Skipping line due to missing field: " << line << std::endl;
            return false;
        }
    }

    try
    {
        student.roll_no = tokens[0];
        student.gender = tokens[1];
        student.race_ethnicity = tokens[2];
        student.parental_level_of_education = tokens[3];
        student.lunch = std::stoi(tokens[4]);
        student.test_preparation_course = std::stoi(tokens[5]);
        student.math_score = std::stoi(tokens[6]);
        student.science_score = std::stoi(tokens[7]);
        student.reading_score = std::stoi(tokens[8]);
        student.writing_score = std::stoi(tokens[9]);
        student.total_score = std::stoi(tokens[10]);
        student.grade = tokens[11][0];
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing line: " << line << "\nException: " << e.what() << std::endl;
        return false;
    }

    return true;
}

// func to calculate Pearson correlation coefficient
double pearson_correlation(const std::vector<double> &X, const std::vector<double> &Y)
{
    if (X.size() != Y.size() || X.empty())
    {
        throw std::invalid_argument("Vectors must be of the same non-zero length.");
    }
    double sum_x = std::accumulate(X.begin(), X.end(), 0.0);
    double sum_y = std::accumulate(Y.begin(), Y.end(), 0.0);
    double sum_x_sq = std::accumulate(X.begin(), X.end(), 0.0, [](double a, double b) { return a + b * b; });
    double sum_y_sq = std::accumulate(Y.begin(), Y.end(), 0.0, [](double a, double b) { return a + b * b; });
    double sum_xy = 0.0;
    for (size_t i = 0; i < X.size(); ++i)
    {
        sum_xy += X[i] * Y[i];
    }
    double numerator = (X.size() * sum_xy) - (sum_x * sum_y);
    double denominator = std::sqrt((X.size() * sum_x_sq - sum_x * sum_x) * (X.size() * sum_y_sq - sum_y * sum_y));
    if (denominator == 0)
        return 0;
    return numerator / denominator;
}

// func to compute linear regression slope and intercept
std::pair<double, double> linear_regression(const std::vector<double> &X, const std::vector<double> &Y)
{
    if (X.size() != Y.size() || X.empty())
    {
        throw std::invalid_argument("Vectors must be of the same non-zero length.");
    }

    double sum_x = std::accumulate(X.begin(), X.end(), 0.0);
    double sum_y = std::accumulate(Y.begin(), Y.end(), 0.0);
    double sum_x_sq = std::accumulate(X.begin(), X.end(), 0.0, [](double a, double b) { return a + b * b; });
    double sum_xy = 0.0;
    for (size_t i = 0; i < X.size(); ++i)
    {
        sum_xy += X[i] * Y[i];
    }

    double n = static_cast<double>(X.size());
    double denominator = (n * sum_x_sq - sum_x * sum_x);
    if (denominator == 0)
    {
        throw std::runtime_error("Denominator in linear regression calculation is zero.");
    }

    double slope = (n * sum_xy - sum_x * sum_y) / denominator;
    double intercept = (sum_y - slope * sum_x) / n;

    return {slope, intercept};
}

int main()
{
    std::vector<Student> students;
    std::ifstream file("data.csv");

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file 'data.csv'!" << std::endl;
        return 1;
    }

    std::string line;
    if (!std::getline(file, line))
    {
        std::cerr << "Error: CSV file is empty!" << std::endl;
        return 1;
    }

    // read each line
    while (std::getline(file, line))
    {
        Student s;
        if (parse_line(line, s))
        {
            students.push_back(s);
        }
        // skip invalid lines
    }

    file.close();

    std::cout << "Total students loaded: " << students.size() << std::endl;

    // extract relevant data for correlation
    std::vector<double> math_scores, science_scores, reading_scores, writing_scores;
    std::vector<std::string> genders;
    std::vector<std::string> race_ethnicities;
    std::vector<int> test_prep;
    std::vector<double> total_scores;
    std::vector<char> grades;

    for (const auto &s : students)
    {
        math_scores.push_back(static_cast<double>(s.math_score));
        science_scores.push_back(static_cast<double>(s.science_score));
        reading_scores.push_back(static_cast<double>(s.reading_score));
        writing_scores.push_back(static_cast<double>(s.writing_score));
        genders.push_back(s.gender);
        race_ethnicities.push_back(s.race_ethnicity);
        test_prep.push_back(s.test_preparation_course);
        total_scores.push_back(static_cast<double>(s.total_score));
        grades.push_back(s.grade);
    }

    // calc Pearson Correlations
    double corr_math_science = 0.0;
    double corr_reading_writing = 0.0;
    double corr_math_total = 0.0;
    double corr_science_total = 0.0;
    double corr_reading_total = 0.0;

    try // can't tell if I like this formatting or not (default VS Code formatting)
    {
        corr_math_science = pearson_correlation(math_scores, science_scores);
        corr_reading_writing = pearson_correlation(reading_scores, writing_scores);
        corr_math_total = pearson_correlation(math_scores, total_scores);
        corr_science_total = pearson_correlation(science_scores, total_scores);
        corr_reading_total = pearson_correlation(reading_scores, total_scores);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error calculating correlations: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Correlation between Math and Science scores: "
        << corr_math_science << std::endl;
    std::cout << "Correlation between Reading and Writing scores: "
        << corr_reading_writing << std::endl;
    std::cout << "Correlation between Math and Total scores: "
        << corr_math_total << std::endl;
    std::cout << "Correlation between Science and Total scores: "
        << corr_science_total << std::endl;
    std::cout << "Correlation between Reading and Total scores: "
        << corr_reading_total << std::endl;

    return 0;

    // linear regression for Math vs. Science
    double slope_math_science = 0.0;
    double intercept_math_science = 0.0;
    try
    {
        auto lr_math_science = linear_regression(math_scores, science_scores);
        slope_math_science = lr_math_science.first;
        intercept_math_science = lr_math_science.second;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error computing linear regression for Math vs. Science: "
            << e.what() << std::endl;
    }

    // linear regression for Reading vs. Writing
    double slope_reading_writing = 0.0;
    double intercept_reading_writing = 0.0;
    try
    {
        auto lr_reading_writing = linear_regression(reading_scores, writing_scores);
        slope_reading_writing = lr_reading_writing.first;
        intercept_reading_writing = lr_reading_writing.second;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error computing linear regression for Reading vs. Writing: "
            << e.what() << std::endl;
    }

    // linear regression for Math vs. Total
    double slope_math_total = 0.0;
    double intercept_math_total = 0.0;
    try
    {
        auto lr_math_total = linear_regression(math_scores, total_scores);
        slope_math_total = lr_math_total.first;
        intercept_math_total = lr_math_total.second;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error computing linear regression for Math vs. Total: "
            << e.what() << std::endl;
    }

    // linear regression for Science vs. Total
    double slope_science_total = 0.0;
    double intercept_science_total = 0.0;
    try
    {
        auto lr_science_total = linear_regression(science_scores, total_scores);
        slope_science_total = lr_science_total.first;
        intercept_science_total = lr_science_total.second;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error computing linear regression for Science vs. Total: "
            << e.what() << std::endl;
    }

    // linear regression for Reading vs. Total
    double slope_reading_total = 0.0;
    double intercept_reading_total = 0.0;
    try
    {
        auto lr_reading_total = linear_regression(reading_scores, total_scores);
        slope_reading_total = lr_reading_total.first;
        intercept_reading_total = lr_reading_total.second;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error computing linear regression for Reading vs. Total: "
            << e.what() << std::endl;
    }

    // Scatter Plot: Math vs. Science + regression line
    plt::figure(true);
    plt::scatter(math_scores, science_scores, 10.0);

    // regression line
    std::vector<double> regression_x_math_science;
    std::vector<double> regression_y_math_science;
    size_t num_points = 100;
    double min_math = *std::min_element(math_scores.begin(), math_scores.end());
    double max_math = *std::max_element(math_scores.begin(), math_scores.end());
    double step_math = (max_math - min_math) / (num_points - 1);

    for (size_t i = 0; i < num_points; ++i)
    {
        double x = min_math + i * step_math;
        double y = slope_math_science * x + intercept_math_science;
        regression_x_math_science.push_back(x);
        regression_y_math_science.push_back(y);
    }

    auto ax1 = plt::gca();
    ax1->hold(true);
    plt::plot(regression_x_math_science, regression_y_math_science, "r-");
    plt::title("Math vs. Science Scores");
    plt::xlabel("Math Scores");
    plt::ylabel("Science Scores");
    plt::legend({"Data Points", "Linear Regression"});
    plt::save("math_science_scatter.png");
    ax1->hold(false);

    // Scatter Plot: Reading vs. Writing + regression line
    plt::figure(true);
    plt::scatter(reading_scores, writing_scores, 10.0, std::string("blue"));

    // regression line
    std::vector<double> regression_x_reading_writing;
    std::vector<double> regression_y_reading_writing;
    double min_reading = *std::min_element(reading_scores.begin(), reading_scores.end());
    double max_reading = *std::max_element(reading_scores.begin(), reading_scores.end());
    double step_reading = (max_reading - min_reading) / (num_points - 1);

    for (size_t i = 0; i < num_points; ++i)
    {
        double x = min_reading + i * step_reading;
        double y = slope_reading_writing * x + intercept_reading_writing;
        regression_x_reading_writing.push_back(x);
        regression_y_reading_writing.push_back(y);
    }

    auto ax2 = plt::gca();
    ax2->hold(true);
    plt::plot(regression_x_reading_writing, regression_y_reading_writing, "r-");
    plt::title("Reading vs. Writing Scores");
    plt::xlabel("Reading Scores");
    plt::ylabel("Writing Scores");
    plt::legend({"Data Points", "Linear Regression"});
    plt::save("reading_writing_scatter.png");
    ax2->hold(false);

    // Scatter Plot: Math vs. Total Scores + regression line
    plt::figure(true);
    plt::scatter(math_scores, total_scores, 10.0, std::string("green"));

    // regression line
    std::vector<double> regression_x_math_total;
    std::vector<double> regression_y_math_total;
    double min_math_total = *std::min_element(math_scores.begin(), math_scores.end());
    double max_math_total = *std::max_element(math_scores.begin(), math_scores.end());
    double step_math_total = (max_math_total - min_math_total) / (num_points - 1);

    for (size_t i = 0; i < num_points; ++i)
    {
        double x = min_math_total + i * step_math_total;
        double y = slope_math_total * x + intercept_math_total;
        regression_x_math_total.push_back(x);
        regression_y_math_total.push_back(y);
    }

    auto ax3 = plt::gca();
    ax3->hold(true);
    plt::plot(regression_x_math_total, regression_y_math_total, "r-");
    plt::title("Math Scores vs. Total Scores");
    plt::xlabel("Math Scores");
    plt::ylabel("Total Scores");
    plt::legend({"Data Points", "Linear Regression"});
    plt::save("math_total_scatter.png");
    ax3->hold(false);

    // Scatter Plot: Science vs. Total Scores + regression line
    plt::figure(true);
    plt::scatter(science_scores, total_scores, 10.0, std::string("purple"));

    // regression line
    std::vector<double> regression_x_science_total;
    std::vector<double> regression_y_science_total;
    double min_science_total = *std::min_element(science_scores.begin(), science_scores.end());
    double max_science_total = *std::max_element(science_scores.begin(), science_scores.end());
    double step_science_total = (max_science_total - min_science_total) / (num_points - 1);

    for (size_t i = 0; i < num_points; ++i)
    {
        double x = min_science_total + i * step_science_total;
        double y = slope_science_total * x + intercept_science_total;
        regression_x_science_total.push_back(x);
        regression_y_science_total.push_back(y);
    }

    auto ax4 = plt::gca();
    ax4->hold(true);
    plt::plot(regression_x_science_total, regression_y_science_total, "r-");
    plt::title("Science Scores vs. Total Scores");
    plt::xlabel("Science Scores");
    plt::ylabel("Total Scores");
    plt::legend({"Data Points", "Linear Regression"});
    plt::save("science_total_scatter.png");
    ax4->hold(false);

    // Scatter Plot: Reading vs. Total Score + regression line
    plt::figure(true);
    plt::scatter(reading_scores, total_scores, 10.0, std::string("orange"));

    // regression line
    std::vector<double> regression_x_reading_total;
    std::vector<double> regression_y_reading_total;
    double min_reading_total = *std::min_element(reading_scores.begin(), reading_scores.end());
    double max_reading_total = *std::max_element(reading_scores.begin(), reading_scores.end());
    double step_reading_total = (max_reading_total - min_reading_total) / (num_points - 1);

    for (size_t i = 0; i < num_points; ++i)
    {
        double x = min_reading_total + i * step_reading_total;
        double y = slope_reading_total * x + intercept_reading_total;
        regression_x_reading_total.push_back(x);
        regression_y_reading_total.push_back(y);
    }

    auto ax5 = plt::gca();
    ax5->hold(true);
    plt::plot(regression_x_reading_total, regression_y_reading_total, "r-");
    plt::title("Reading Scores vs. Total Scores");
    plt::xlabel("Reading Scores");
    plt::ylabel("Total Scores");
    plt::legend({"Data Points", "Linear Regression"});
    plt::save("reading_total_scatter.png");
    ax5->hold(false);

    std::cout << "Plots have been saved successfully." << std::endl;

    return 0;
}