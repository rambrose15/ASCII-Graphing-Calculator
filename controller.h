#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <variant>
#include <ncurses.h>
#include <map>

enum KeyPress{
  UPARROW, DOWNARROW, LEFTARROW, RIGHTARROW,
  ESC, ENTER, BACKSPACE, DELETE, NONE
};

class Controller{
  std::map<int,KeyPress> keyMapping = {
    {ERR, NONE}, {KEY_UP, UPARROW}, {KEY_DOWN, DOWNARROW}, {KEY_LEFT, LEFTARROW},
    {KEY_RIGHT, RIGHTARROW}, {KEY_ENTER, ENTER}, {KEY_BACKSPACE, BACKSPACE},
    {KEY_DL, DELETE}, {27, ESC}
  };

  public:
    Controller();
    std::variant<char,KeyPress> getInput();
    std::pair<int,int> getScreenSize();
};

#endif
