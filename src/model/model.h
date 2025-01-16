#ifndef __MODEL_H__
#define __MODEL_H__

#include <memory>
#include <string>
#include <variant>

#include "../view/view.h"
#include "../controller/controller.h"
#include "formula/formula_list.h"
#include "formula/screen_info.h"
#include "../utility/colour.h"

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
    ScreenInfo* screenInfo;
    bool commandMode;

    int maxRow, maxCol;

    std::string currentCommand, commandMessage;
    int commandCursorIndex, commandCursorPosition;
    bool commandError;

    virtual void runInsideCommand() = 0;
    virtual void runOutsideCommand() = 0;

    void updateCommand(std::variant<char,KeyPress> nextChar);
    void processCommandDefault();
    void displayCommandError(std::string message);
    void displayCommandMessage(std::string message);

    virtual bool processCommandSpecific(std::vector<std::string> cmdWords) = 0;
    virtual void initializeSpecific() = 0;

    virtual void onColourChange(int index) = 0;
    virtual void onScreenSizeChange() = 0;
    virtual void onScreenCoordsChange() {}
    virtual void onCommandExecute() {}

  public:

  Model(View* view, Controller* controller, FormulaList* formulas, ScreenInfo* screenInfo): 
    view{view}, controller{controller}, formulas{formulas},
    screenInfo{screenInfo}, commandMode{true} {}

  ExitStatus runModel();
};

#endif
