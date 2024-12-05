#ifndef __FORMULA_MODEL_H__
#define __FORMULA_MODEL_H__

#include "model.h"

class FormulaModel : public Model{
  public:
    FormulaModel(View* v, Controller* c, FormulaList* f): Model{v,c,f} {}
    bool processCommandSpecific() override;
};

#endif
