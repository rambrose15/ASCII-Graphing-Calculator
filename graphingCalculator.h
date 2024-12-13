#ifndef __GRAPHING_CALCULATOR_H__
#define __GRAPHING_CALCULATOR_H__

#include <memory>
#include <ncurses.h>

#include "view.h"
#include "controller.h"
#include "formulaList.h"
#include "formulaModel.h"
#include "graphingModel.h"
#include "screenInfo.h"

class GraphingCalculator{
  View view;
  Controller controller;
  FormulaList formulas;
  FormulaModel formulaModel;
  GraphingModel graphingModel;
  ScreenInfo screenInfo;

  void configureCurses();

  public:
    GraphingCalculator();
    ~GraphingCalculator();
    void loadBeforeRun(std::string fileName);
    void run();
};

#endif
