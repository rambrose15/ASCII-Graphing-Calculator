#ifndef __GRAPHING_CALCULATOR_H__
#define __GRAPHING_CALCULATOR_H__

#include <memory>
#include <ncurses.h>

#include "view.h"
#include "controller.h"
#include "formulaList.h"
#include "formulaModel.h"
#include "graphingModel.h"

class GraphingCalculator{
  View view;
  Controller controller;
  FormulaList formulas;
  FormulaModel formulaModel;
  GraphingModel graphingModel;

  public:
    GraphingCalculator();
    ~GraphingCalculator();
    void configureCurses();
    void run();
};

#endif
