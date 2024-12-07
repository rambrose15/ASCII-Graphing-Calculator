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
    BigRational screenDimXL, screenDimXR, screenDimYL, screenDimYR;

    std::string currentCommand;
    int commandCursorIndex, commandCursorPosition;

    virtual void runInsideCommand() = 0;
    virtual void runOutsideCommand() = 0;

    void updateCommand(std::variant<char,KeyPress> nextChar);
    void cleanCommand();
    void processCommandDefault();
    void displayCommandError(std::string message);
    void displayCommandMessage(std::string message);

    virtual bool processCommandSpecific(std::vector<std::string> cmdWords) = 0;
    virtual void initializeSpecific() = 0;

    virtual void onColourChange(int index) = 0;

  public:
    Model(View* view, Controller* controller, FormulaList* formulas): 
      view{view}, controller{controller}, formulas{formulas},
      commandMode{true}, 
      screenDimXL{BigRational("-10")}, screenDimXR{BigRational("10")},
      screenDimYL{BigRational("-10")}, screenDimYR{BigRational("10")} {}
    ExitStatus runModel();
};

#endif
