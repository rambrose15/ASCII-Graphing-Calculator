#ifndef __VIEW_H__
#define __VIEW_H__

#include <string>
#include <vector>
#include <map>
#include <ncurses.h>

enum Colour{
  WHITE, BLACK, RED, GREEN, BLUE, YELLOW, PURPLE, CYAN, NOCOLOUR
};

class View{

  int getColour(Colour colour);
  
  public:
    void initializeColours();
    Colour getColourFromString(const std::string& str);
    std::string getStringFromColour(Colour colour);
    void moveCursor(int row, int col);
    void updatePlace(int row, int col, char newChar, Colour = WHITE);
    void updateRow(int row, const std::string& newString, const std::vector<Colour>& colours = {});
    void updateColumn(int col, const std::string& newString, const std::vector<Colour>& colours = {});
    void wipe();
    void updateFull(const std::vector<std::string>& newStrings, 
                    const std::vector<std::vector<Colour>>& colours = {{}});
    void loadScreen();
};

#endif
