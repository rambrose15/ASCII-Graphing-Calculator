#include "controller.h"

using std::variant, std::pair;

variant<char,KeyPress> Controller::getInput(){
  int input = getch();
  switch(input){
    case ERR: return NOPRESS;
    case KEY_BACKSPACE: case 127: return BACKSPACE;
    case KEY_ENTER: case 10: return ENTER;
    case KEY_UP: return UPARROW;
    case KEY_DOWN: return DOWNARROW;
    case KEY_LEFT: return LEFTARROW;
    case KEY_RIGHT: return RIGHTARROW;
    default: return (char)input;;
  }
  return NOPRESS;
};

pair<int,int> Controller::getScreenSize(){
  int r, c;
  getmaxyx(stdscr, r, c);
  return {r,c};
}
