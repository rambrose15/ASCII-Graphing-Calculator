#ifndef __VIEW_H__
#define __VIEW_H__

#include <string>
#include <vector>
#include <map>
#include <ncurses.h>

enum Colour{
  WHITE, BLACK, RED, GREEN, BLUE, YELLOW, PURPLE, CYAN
};

class View{

  std::map<Colour,short> colourMapping = {
    {WHITE, COLOR_WHITE},
    {BLACK, COLOR_BLACK},
    {RED, COLOR_RED},
    {GREEN, COLOR_GREEN},
    {BLUE, COLOR_BLUE},
    {YELLOW, COLOR_YELLOW},
    {PURPLE, COLOR_MAGENTA},
    {CYAN, COLOR_CYAN}
  };
  
  public:
    View();
    void updatePlace(int row, int col, char newChar, Colour = WHITE);
    void updateRow(int row, const std::string& newString, const std::vector<Colour>& colour = {});
    void updateColumn(int col, const std::string& newString, const std::vector<Colour>& colours = {});
    void wipe();
    void updateFull(const std::vector<std::string>& newStrings, 
                    const std::vector<std::vector<Colour>>& colours = {{}});
    void loadScreen();
};

#endif
