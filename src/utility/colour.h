#ifndef __COLOUR_H__
#define __COLOUR_H__

#include <string>

enum Colour{
  WHITE, BLACK, RED, GREEN, BLUE, YELLOW, PURPLE, CYAN, NOCOLOUR
};

// Utilities for dealing with Colours
Colour getColourFromString(const std::string& str);

std::string getStringFromColour(Colour colour);

#endif
