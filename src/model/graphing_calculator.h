#ifndef __GRAPHING_CALCULATOR_H__
#define __GRAPHING_CALCULATOR_H__

#include <memory>
#include <ncurses.h>

#include "../view/view.h"
#include "../controller/controller.h"
#include "formula/formula_list.h"
#include "formula/formula_model.h"
#include "graphing/graphing_model.h"
#include "formula/screen_info.h"

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
