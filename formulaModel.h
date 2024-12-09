#ifndef __FORMULA_MODEL_H__
#define __FORMULA_MODEL_H__

#include "model.h"
#include <vector>
#include <string>

class FormulaModel : public Model{
  std::vector<std::string> stringSet;
  int selectedFormula, formulaCursorIndex;
  const int FORMULA_BUFFER;

  std::string currentFileName;

  void initializeSpecific() override;

  bool processCommandSpecific(std::vector<std::string> cmdWords) override;
  void runInsideCommand() override {};
  void runOutsideCommand() override;

  void onColourChange(int index) override;
  void onScreenSizeChange() override;

  void displayFormulas(int startInd, bool includeErrors = false);
  int displaySingleFormula(int line, int index);

  bool saveToFile(const std::string& fileName);

  bool loadFromFile(const std::string& fileName);

  public:
  
    FormulaModel(View* v, Controller* c, FormulaList* f, ScreenInfo* s): Model{v,c,f,s}, 
      stringSet{std::vector<std::string>(100,"")}, FORMULA_BUFFER{4},
      currentFileName{""} {}
};

#endif
