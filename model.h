#ifndef __MODEL_H__
#define __MODEL_H__

#include <memory>
#include <string>
#include <variant>

#include "view.h"
#include "controller.h"
#include "formulaList.h"

enum ExitStatus{
  NOSTATUS, QUIT, SWITCH
};

class Model{

  void updateScreenDimensions();
  ExitStatus exitStatus = NOSTATUS;

  protected:
    View* view;
    Controller* controller;
    FormulaList* formulas;
    bool commandMode;

    int maxRow, maxCol;

    std::string currentCommand;
    int commandCursorIndex = 0;

    void updateCommand(std::variant<char,KeyPress> nextChar);
    void processCommandDefault();
    virtual void processCommandSpecific() = 0;

  public:
    Model(View* view, Controller* controller, FormulaList* formulas): 
      view{view}, controller{controller}, formulas{formulas},
      commandMode{true} {}
    ExitStatus runModel();
};

#endif
