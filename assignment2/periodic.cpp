#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

struct Element
{
	Element() : symbol(), name(), number(0), atomic_mass(0.0), phase(), type() {}

	std::string symbol; // e.g., "H", "He"
	std::string name;		// e.g., "Hydrogen", "Helium"
	int number;					// e.g., 1, 2
	double atomic_mass; // e.g., 1.008, 4.0026
	std::string phase;	// phase at room temp e.g., gas, liquid, solid
	std::string type;		// e.g., "transitional metals", "noble gas"
};

// Comparator for sorting the index vectors
struct CompClass
{
	bool operator()(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
	{
		return a.first < b.first;
	}
} comp;

int index_find(const std::vector<std::pair<std::string, int> > &index, const std::string &key)
{
	int left = 0;
	int right = static_cast<int>(index.size()) - 1;
	while (left <= right)
	{
		int mid = left + (right - left) / 2;
		if (index[mid].first == key)
		{
			return index[mid].second;
		}
		else if (index[mid].first < key)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}
	return -1; // Not found
}

void display_element(const Element &elem)
{
	std::cout << "Symbol:       " << elem.symbol << std::endl;
	std::cout << "Name:         " << elem.name << std::endl;
	std::cout << "Atomic Number:" << elem.number << std::endl;
	std::cout << "Atomic Mass:  " << elem.atomic_mass << std::endl;
	std::cout << "Phase:        " << elem.phase << std::endl;
	std::cout << "Type:         " << elem.type << std::endl;
}

int main(int argc, char *argv[])
{
	std::vector<Element> elements(119);
	std::vector<std::pair<std::string, int> > name_index;
	std::vector<std::pair<std::string, int> > symbol_index;

	std::ifstream file("elements.csv");
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file 'periodic_table_data.csv'" << std::endl;
		return 1;
	}

	// skip header
	std::string line;
	std::getline(file, line);

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string token;

		Element elem;

		// Atomic Number
		std::getline(ss, token, ',');
		elem.number = std::stoi(token);

		// Name
		std::getline(ss, token, ',');
		elem.name = token;

		// Symbol
		std::getline(ss, token, ',');
		elem.symbol = token;

		// Atomic Mass
		std::getline(ss, token, ',');
		elem.atomic_mass = std::stod(token);

		// Phase
		std::getline(ss, token, ',');
		elem.phase = token;

		// Type
		std::getline(ss, token, ',');
		elem.type = token;

		// Insert into elements vector
		if (elem.number >= 1 && elem.number <= 118)
		{
			elements[elem.number] = elem;
		}
		else
		{
			std::cerr << "Warning: Atomic number out of range: " << elem.number << std::endl;
			continue;
		}

		name_index.push_back(std::make_pair(elem.name, elem.number));

		symbol_index.push_back(std::make_pair(elem.symbol, elem.number));
	}

	file.close();

	std::sort(name_index.begin(), name_index.end(), comp);
	std::sort(symbol_index.begin(), symbol_index.end(), comp);

	while (true)
	{
		std::cout << "Enter element number, symbol, name, or 'quit' to exit: ";
		std::string input;
		std::getline(std::cin, input);

		// trim whitespace
		input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
		input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);

		// convert input to uppercase for case-insensitive comparison
		std::string input_upper = input;
		std::transform(input_upper.begin(), input_upper.end(), input_upper.begin(), ::toupper);

		if (input_upper == "QUIT")
		{
			break;
		}

		// check if input is a number
		bool is_number = !input.empty() && std::all_of(input.begin(), input.end(), ::isdigit);

		if (is_number)
		{
			int atomic_number = std::stoi(input);
			if (atomic_number >= 1 && atomic_number <= 118)
			{
				Element elem = elements[atomic_number];
				if (!elem.name.empty())
				{
					display_element(elem);
				}
				else
				{
					std::cout << "Element not found." << std::endl;
				}
			}
			else
			{
				std::cout << "Invalid atomic number. Please enter a number between 1 and 118." << std::endl;
			}
		}
		else
		{
			// determine if input is a symbol or name
			if (input.length() <= 2)
			{
				// assume symbol
				// convert symbol to proper case (first letter uppercase, rest lowercase)
				std::string symbol = input;
				symbol[0] = toupper(symbol[0]);
				if (symbol.length() == 2)
				{
					symbol[1] = tolower(symbol[1]);
				}

				int atomic_number = index_find(symbol_index, symbol);
				if (atomic_number != -1)
				{
					Element elem = elements[atomic_number];
					display_element(elem);
				}
				else
				{
					std::cout << "Element not found." << std::endl;
				}
			}
			else
			{
				// assume name
				// capitalize the first letter and make the rest lowercase
				std::string name = input;
				name[0] = toupper(name[0]);
				for (size_t i = 1; i < name.length(); ++i)
				{
					name[i] = tolower(name[i]);
				}

				int atomic_number = index_find(name_index, name);
				if (atomic_number != -1)
				{
					Element elem = elements[atomic_number];
					display_element(elem);
				}
				else
				{
					std::cout << "Element not found." << std::endl;
				}
			}
		}

		std::cout << std::endl; // add an empty line for readability
	}

	std::cout << "Goodbye!" << std::endl;
	return 0;
}