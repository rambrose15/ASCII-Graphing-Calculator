#include "view.h"

using std::string, std::vector;

void View::moveCursor(int row, int col) { move(row, col); }

void View::updatePlace(int row, int col, char newChar, Colour colour){
  attron(COLOR_PAIR(getColour(colour)));
  mvaddch(row, col, newChar);
  attroff(getColour(colour));
}

void View::updateRow(int row, const std::string& newString, const std::vector<Colour>& colours){
  int maxR, maxC;
  getmaxyx(stdscr, maxR, maxC);
  if (row >= maxR || row < 0) return;
  int strLen = newString.length(), colourLen = colours.size();
  for (int col = 0; col < maxC; col++){
    if (col < strLen && col < colourLen) updatePlace(row, col, newString[col], colours[col]);
    else if (col < strLen) updatePlace(row, col, newString[col]);
    else updatePlace(row, col, ' ');
  }
}

void View::updateColumn(int col, const std::string& newString, const std::vector<Colour>& colours){
  int maxR, maxC;
  getmaxyx(stdscr, maxR, maxC);
  if (col >= maxC || col < 0) return;
  int strLen = newString.length(), colourLen = colours.size();
  for (int row = 0; row < maxR; row++){
    if (row < strLen && row < colourLen) updatePlace(row, col, newString[row], colours[row]);
    else if (row < strLen) updatePlace(row, col, newString[row]);
    else updatePlace(row, col, ' ');
  }
}

void View::wipe(){
  int maxR, maxC;
  getmaxyx(stdscr, maxR, maxC);
  updateFull(vector(maxR, string(maxC, ' ')));
}

void View::updateFull(const std::vector<std::string>& newStrings, const std::vector<std::vector<Colour>>& colours){
  int maxR = getmaxy(stdscr);
  int strLen = newStrings.size(), colourLen = colours.size();
  for (int row = 0; row < maxR; row++){
    if (row < strLen && row < colourLen) updateRow(row, newStrings[row], colours[row]);
    else if (row < strLen) updateRow(row, newStrings[row]);
    else updateRow(row, "");
  }
}

void View::loadScreen(){
  refresh();
}

int View::getColour(Colour colour){
  switch(colour){
    case WHITE: return 1;
    case BLACK: return 2;
    case RED: return 3;
    case GREEN: return 4;
    case BLUE: return 5;
    case YELLOW: return 6;
    case PURPLE: return 7;
    case CYAN: return 8;
    default: return 1;
  }
  return 1;
}

void View::initializeColours(){
  init_pair(getColour(WHITE), COLOR_WHITE, COLOR_BLACK);
  init_pair(getColour(BLACK), COLOR_BLACK, COLOR_WHITE);
  init_pair(getColour(RED), COLOR_RED, COLOR_BLACK);
  init_pair(getColour(BLUE), COLOR_BLUE, COLOR_BLACK);
  init_pair(getColour(GREEN), COLOR_GREEN, COLOR_BLACK);
  init_pair(getColour(YELLOW), COLOR_YELLOW, COLOR_BLACK);
  init_pair(getColour(CYAN), COLOR_CYAN, COLOR_BLACK);
  init_pair(getColour(PURPLE), COLOR_MAGENTA, COLOR_BLACK);
}

Colour View::getColourFromString(const string& str){
  if (str == "w" || str == "white") return WHITE;
  else if (str == "r" || str == "red") return RED;
  else if (str == "b" || str == "blue") return BLUE;
  else if (str == "g" || str == "green") return GREEN;
  else if (str == "y" || str == "yellow") return YELLOW;
  else if (str == "c" || str == "cyan") return CYAN;
  else if (str == "p" || str == "purple") return PURPLE;
  else if (str == "n" || str == "none") return NOCOLOUR;
  return BLACK;
}

string View::getStringFromColour(Colour colour){
  switch(colour){
    case WHITE: return "w";
    case RED: return "r";
    case BLUE: return "b";
    case GREEN: return "g";
    case YELLOW: return "y";
    case CYAN: return "c";
    case PURPLE: return "p";
    case NOCOLOUR: return "n";
    default: return "";
  } return "";
}