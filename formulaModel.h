#ifndef __FORMULA_MODEL_H__
#define __FORMULA_MODEL_H__

#include "model.h"

class FormulaModel : public Model{
  void initializeSpecific() override;

  bool processCommandSpecific() override;
  void runInsideCommand() override {};
  void runOutsideCommand() override;

  void preLoad();

  public:
    FormulaModel(View* v, Controller* c, FormulaList* f): Model{v,c,f} {}
};

#endif
