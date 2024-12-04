#include "view.h"

using std::string, std::vector;

void View::moveCursor(int row, int col) { move(row, col); }

void View::updatePlace(int row, int col, char newChar, Colour colour){
  init_pair(1, colourMapping[colour], COLOR_BLACK);
  attron(COLOR_PAIR(1));
  mvaddch(row, col, newChar);
  //mvaddch(row, col, 'X');
  attroff(COLOR_PAIR(1));
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
