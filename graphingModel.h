#ifndef __GRAPHING_MODEL_H__
#define __GRAPHING_MODEL_H__

#include "model.h"

class GraphingModel : public Model{
  public:
    GraphingModel(View* v, Controller* c, FormulaList* f): Model{v,c,f} {}
    void processCommandSpecific() override;
};

#endif
