#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cctype>

using namespace std;

const string GREEN = "\033[92m";
const string YELLOW = "\033[93m";
const string COLOR_OFF = "\033[0m";

// declaration for play_game (needed for ask_for_new_game)
void play_game(const vector<string>& word_list);

struct Cell {
  char letter;
  string color;

  Cell() : letter('*'), color(COLOR_OFF) {}

  Cell(char l, string c) : letter(l), color(c) {}
};

void print_board(const vector<vector<Cell> >& board) {
  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < board[i].size(); ++j) {
      cout << board[i][j].color << board[i][j].letter << COLOR_OFF;
    }
    cout << endl;
  }
}

string to_lowercase(const string& str) {
  string lower_str = str;
  for (size_t i = 0; i < lower_str.length(); ++i) {
    lower_str[i] = tolower(lower_str[i]);
  }
  return lower_str;
}

vector<string> read_word_list(const string& filename) {
  vector<string> word_list;
  ifstream file(filename);

  if (!file.is_open()) {
    cerr << "Error: Could not open the file " << filename << endl;
    exit(1);
  }

  string word;
  while (getline(file, word)) {
    string lower_word = to_lowercase(word);
    word_list.push_back(word);
  }

  file.close();
  return word_list;
}

bool is_valid_word(const string& word, const vector<string>& word_list) {
  string lower_word = to_lowercase(word);
  return binary_search(word_list.begin(), word_list.end(), lower_word);
}


void update_board(vector<vector<Cell> >& board, const string& guess, const string& target, int attempt) {
  for (size_t i = 0; i < guess.length(); ++i) {
    if (guess[i] == target[i]) {
      board[attempt][i] = Cell(guess[i], GREEN);
    } else if (target.find(guess[i]) != string::npos) {
      board[attempt][i] = Cell(guess[i], YELLOW);
    } else {
      board[attempt][i] = Cell(guess[i], COLOR_OFF);
    }
  }
}

void print_word_in_green(const string& word) {
  for (size_t i = 0; i < word.length(); ++i) {
    cout << GREEN << word[i] << COLOR_OFF;
  }
  cout << endl;
}

void ask_for_new_game(const vector<string>& word_list) {
  string input;
  while (true) {
    cout << "Do you want to start a new game (y/n)? ";
    cin >> input;
    if (input == "y" || input == "Y") {
      play_game(word_list);
      break;
    } else if (input == "n" || input == "N") {
      cout << "Thanks for playing!" << endl;
      exit(0);
    } else {
      cout << "Invalid input. Please enter 'y' or 'n'." << endl;
    }
  }
}

void play_game(const vector<string>& word_list) {
  string target_word = word_list[rand() % word_list.size()];

#ifndef DEBUG
  cout << "Target word: " << target_word << endl;
#endif

  vector<vector<Cell> > board(6, vector<Cell>(5));

  for (int attempt = 0; attempt < 6; ++attempt) {
    print_board(board);

    cout << "Enter your guess: ";
    string guess;
    cin >> guess;

    if (guess == "quit") {
      cout << "Thanks for playing!" << endl;
      return;
    } else if (guess == "new") {
      cout << "Starting a new game..." << endl;
      play_game(word_list);
      return;
    } else if (guess.length() != 5 || !is_valid_word(guess, word_list)) {
      cout << "Invalid word, please try again." << endl;
      --attempt;
      continue;
    }

    update_board(board, guess, target_word, attempt);

    if (guess == target_word) {
      print_board(board);
      cout << "Congratulations, you've guessed the word! The word was: ";
      print_word_in_green(target_word);

      ask_for_new_game(word_list);
      return;
    }
  }

  // out of attempts
  print_board(board);
  cout << "Sorry, you've used all attempts. The correct word was: " << GREEN << target_word << COLOR_OFF << endl;

  ask_for_new_game(word_list);
}

int main() {
  srand(static_cast<unsigned int>(time(0)));

  vector<string> word_list = read_word_list("wordlist.txt");

  sort(word_list.begin(), word_list.end());

  cout << "Welcome to Wordle!" << endl;

  play_game(word_list);

  return 0;
}