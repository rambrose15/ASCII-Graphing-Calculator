#include "controller.h"
#include <iostream>

using std::variant, std::pair;

variant<char,KeyPress> Controller::getInput(){
  int input = getch();
  if (keyMapping.find(input) != keyMapping.end()){
    return keyMapping[input];
  }
  return (char)input;
};

pair<int,int> Controller::getScreenSize(){
  int r, c;
  getmaxyx(stdscr, r, c);
  return {r,c};
}