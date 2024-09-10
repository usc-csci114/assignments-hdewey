#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

struct Point {
  double x, y;
};

int main() {

  Point points[3]; 
  string input;
  int count = 0;

  cout << "Welcome to triangle area calculator" << endl;

  cout << "Enter 3 sets of doubles for points ('quit' to exit):" << endl;

  while (count < 3) {
    cout << "Enter point " << count + 1 << " (x and y separated by space): ";

    cin >> input;
    if (input == "quit") {
      cout << "Exiting..." << endl;
      return 0;
    }

    cin.putback(input[0]);

    cin >> points[count].x >> points[count].y;

    if (cin.fail()) {
      cin.clear(); 
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid input, try again." << endl;
    } else {
      count++;
    }
  }

  // Formula to calc triangle
  // |(1/2)*(x1(y2 − y3) + x2(y3 − y1) + x3(y1 − y2)|

  double a, b, c, d, area;

  a = points[1].y - points[2].y;

  b = points[2].y - points[0].y;

  c = points[0].y - points[1].y;

  d = 0.5 * ((points[0].x * a) + (points[1].x * b) + (points[2].x * c));

  area = abs(d);

  cout << "The area of your triangle is: " << area << endl;
};
