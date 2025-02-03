#include "colour.h"

Colour getColourFromString(const std::string& str){
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

std::string getStringFromColour(Colour colour){
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
