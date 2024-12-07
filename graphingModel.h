#ifndef __GRAPHING_MODEL_H__
#define __GRAPHING_MODEL_H__

#include "model.h"

class GraphingModel : public Model{
  GraphPackage gp;
  
  void initializeSpecific() override;
  void graphFunctions();
  void updateGP();

  bool processCommandSpecific(std::vector<std::string> cmdWords) override;
  void runInsideCommand() override;
  void runOutsideCommand() override;

  void onColourChange(int index) override;
  void onScreenSizeChange() override;

  public:
    GraphingModel(View* v, Controller* c, FormulaList* f): Model{v,c,f} {}
};

#endif
