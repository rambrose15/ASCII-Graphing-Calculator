#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <variant>
#include <ncurses.h>
#include <map>

enum KeyPress{
  UPARROW, DOWNARROW, LEFTARROW, RIGHTARROW,
  ESC, ENTER, BACKSPACE, DELETE, NOPRESS
};

class Controller{

  public:
    std::variant<char,KeyPress> getInput();
    std::pair<int,int> getScreenSize();
};

#endif
