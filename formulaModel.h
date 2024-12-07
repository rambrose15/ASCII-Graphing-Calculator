#ifndef __FORMULA_MODEL_H__
#define __FORMULA_MODEL_H__

#include "model.h"
#include <vector>
#include <string>

class FormulaModel : public Model{
  std::vector<std::string> stringSet;
  int selectedFormula, formulaCursorIndex;

  void initializeSpecific() override;

  bool processCommandSpecific(std::vector<std::string> cmdWords) override;
  void runInsideCommand() override {};
  void runOutsideCommand() override;

  void displayFormulas(int startInd);
  int displaySingleFormula(int line, int index);

  void preLoad();

  public:
    FormulaModel(View* v, Controller* c, FormulaList* f): Model{v,c,f} {}
};

#endif
