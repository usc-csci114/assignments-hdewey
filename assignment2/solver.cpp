#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "system.h"

using namespace std;

int main(int argc, char *argv[])
{
  int n, m;
  cout << "Enter the number of equations (n): ";
  cin >> n;
  cout << "Enter the number of variables (m): ";
  cin >> m;

  cin.ignore();

  vector<vector<double> > matrix(n, vector<double>(m + 1));

  for (int i = 0; i < n; ++i)
  {
    while (true)
    {
      cout << "Enter equation " << (i + 1) << " coefficients (a_ij and b_i), separated by spaces: ";
      string line;
      getline(cin, line);
      istringstream iss(line);
      vector<double> row;
      double val;
      while (iss >> val)
      {
        row.push_back(val);
      }
      if (row.size() == m + 1)
      {
        matrix[i] = row;
        break;
      }
      else
      {
        cout << "Error: Expected " << (m + 1) << " coefficients, but got " << row.size() << ". Please try again..." << endl;
      }
    }
  }

  System sys(n, m, matrix);
  sys.solve();
  int num_solutions = sys.getNumSolutions();

  if (num_solutions == 0)
  {
    cout << "The system has no solution..." << endl;
  }
  else if (num_solutions == 1)
  {
    cout << "The system has a unique solution:" << endl;
    vector<double> sol = sys.getSolution();
    for (size_t i = 0; i < sol.size(); ++i)
    {
      cout << "x" << (i + 1) << " = " << sol[i] << endl;
    }
  }
  else if (num_solutions == 2)
  {
    cout << "The system has infinitely many solutions. One possible solution is:" << endl;
    vector<double> sol = sys.getSolution();
    for (size_t i = 0; i < sol.size(); ++i)
    {
      cout << "x" << (i + 1) << " = " << sol[i] << endl;
    }
  }
  else
  {
    cout << "Error: Unknown number of solutions..." << endl;
  }

  return 0;
}