#include "formulaModel.h"

using std::vector,std::string;

bool FormulaModel::processCommandSpecific() {
  // Formula window command interpretations
  return false;
}

void FormulaModel::runOutsideCommand(){

}

void FormulaModel::initializeSpecific() {
  preLoad();
}

void FormulaModel::preLoad(){ // Mostly for debugging purposes
  vector<string> strs = {"f(x) = x^2-8"};
  for (int ind = 0, n = strs.size(); ind < n; ind++){
    formulas->updateFormula(ind+1, strs[ind]);
  }
}
