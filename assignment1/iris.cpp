#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>

using namespace std;

struct IrisRecord {
  double sepal_length;
  double sepal_width;
  double petal_length;
  double petal_width;
  string species;
};

struct SpeciesStats {
  vector<double> sepal_length, sepal_width, petal_length, petal_width;
};

// function to compute min, max, mean, and std. deviation
void compute_stats(const vector<double> &data, double &min, double &max, double &mean, double &stddev) {
  min = *min_element(data.begin(), data.end());
  max = *max_element(data.begin(), data.end());
  mean = accumulate(data.begin(), data.end(), 0.0) / data.size();
  double sq_sum = inner_product(data.begin(), data.end(), data.begin(), 0.0);
  stddev = sqrt(sq_sum / data.size() - mean * mean);
}

// print one row of stats for a species
void print_row(const string &species_name, SpeciesStats &stats) {
  double min, max, mean, stddev;
  cout << "| " << setw(20) << left << species_name << " |";

  int w_size = 4;

  // sepal_length
  compute_stats(stats.sepal_length, min, max, mean, stddev);
  cout << setw(w_size) << fixed << setprecision(2) << min << ", "
    << setw(w_size) << max << ", "
    << setw(w_size) << mean << ", "
    << setw(w_size) << stddev << " |";

  // sepal_width
  compute_stats(stats.sepal_width, min, max, mean, stddev);
  cout << setw(w_size) << fixed << setprecision(2) << min << ", "
    << setw(w_size) << max << ", "
    << setw(w_size) << mean << ", "
    << setw(w_size) << stddev << " |";

  // petal_length
  compute_stats(stats.petal_length, min, max, mean, stddev);
  cout << setw(w_size) << fixed << setprecision(2) << min << ", "
    << setw(w_size) << max << ", "
    << setw(w_size) << mean << ", "
    << setw(w_size) << stddev << " |";

  // petal_width
  compute_stats(stats.petal_width, min, max, mean, stddev);
  cout << setw(w_size) << fixed << setprecision(2) << min << ", "
    << setw(w_size) << max << ", "
    << setw(w_size) << mean << ", "
    << setw(w_size) << stddev << " |" << endl;
}

// print the table of stats
void print_table(SpeciesStats &setosa, SpeciesStats &versicolor, SpeciesStats &virginica) {
  cout << setw(60) << "Iris Data" << endl;
  cout << string(120, '-') << endl;
  cout << "|      Species         | Sepal Length          | Sepal Width           | Petal Length          | Petal Width           |" << endl;
  cout << string(120, '-') << endl;

  print_row("Iris-setosa", setosa);
  cout << string(120, '-') << endl;
  print_row("Iris-versicolor", versicolor);
  cout << string(120, '-') << endl;
  print_row("Iris-virginica", virginica);
  cout << string(120, '-') << endl;
}

int main() {
  ifstream infile("iris.txt");
  if (!infile) {
    cerr << "Error opening iris.txt" << endl;
    return 1;
  }

  vector<IrisRecord> records;
  string line;
  while (getline(infile, line)) {
    stringstream ss(line);
    string value;
    IrisRecord record;
    getline(ss, value, ',');
    record.sepal_length = atof(value.c_str());
    getline(ss, value, ',');
    record.sepal_width = atof(value.c_str());
    getline(ss, value, ',');
    record.petal_length = atof(value.c_str());
    getline(ss, value, ',');
    record.petal_width = atof(value.c_str());
    getline(ss, value, ',');
    record.species = value;
    records.push_back(record);
  }

  SpeciesStats setosa, versicolor, virginica;

  // group by species
  for (size_t i = 0; i < records.size(); ++i) {
    if (records[i].species == "Iris-setosa") {
      setosa.sepal_length.push_back(records[i].sepal_length);
      setosa.sepal_width.push_back(records[i].sepal_width);
      setosa.petal_length.push_back(records[i].petal_length);
      setosa.petal_width.push_back(records[i].petal_width);
    }
    else if (records[i].species == "Iris-versicolor") {
      versicolor.sepal_length.push_back(records[i].sepal_length);
      versicolor.sepal_width.push_back(records[i].sepal_width);
      versicolor.petal_length.push_back(records[i].petal_length);
      versicolor.petal_width.push_back(records[i].petal_width);
    }
    else if (records[i].species == "Iris-virginica") {
      virginica.sepal_length.push_back(records[i].sepal_length);
      virginica.sepal_width.push_back(records[i].sepal_width);
      virginica.petal_length.push_back(records[i].petal_length);
      virginica.petal_width.push_back(records[i].petal_width);
    }
  }

  print_table(setosa, versicolor, virginica);

  return 0;
}