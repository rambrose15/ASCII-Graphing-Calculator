#include "graphing_calculator.h"

// Mediatory between the view, controller, and model components.
// Responsible for initializing, configuring, and closing the curses library.
// Used to pass control between the formula model and graphing model.

GraphingCalculator::GraphingCalculator(): 
  view{}, controller{}, formulas{},
  formulaModel{&view, &controller, &formulas, &screenInfo},
  graphingModel{&view, &controller, &formulas, &screenInfo},
  screenInfo{} {
    configureCurses();
}

GraphingCalculator::~GraphingCalculator(){
  endwin();
}

void GraphingCalculator::configureCurses(){
  initscr();
  setlocale(LC_ALL, "");
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  ESCDELAY = 10;
  start_color();
  view.initializeColours();
}

void GraphingCalculator::loadBeforeRun(std::string fileName){
  formulaModel.loadFromFile(fileName);
}

void GraphingCalculator::run(){
  ExitStatus status = NOSTATUS;
  while (status != QUIT){
    status = formulaModel.runModel();
    if (status == QUIT) break;
    status = graphingModel.runModel();
  }
}
