#ifndef __GRAPHING_MODEL_H__
#define __GRAPHING_MODEL_H__

#include "model.h"
#include <set>
#include <chrono>
#include <ctime>

class GraphingModel : public Model{
  GraphPackage gp;
  std::vector<BigRational> vLineSet, hLineSet;
  std::set<int> hidden;
  std::set<int> playing;

  std::chrono::system_clock::time_point startTime;
  inline static const double UPDATE_TIME = 1.0;
  
  void initializeSpecific() override;
  void graphFunctions();
  void graphLines(std::vector<std::string>& graphScreen);
  void updateGP();
  void parameterUpdate(const std::set<int>& params);

  bool processCommandSpecific(std::vector<std::string> cmdWords) override;
  void runInsideCommand() override;
  void runOutsideCommand() override;

  void onColourChange(int index) override;
  void onScreenSizeChange() override;

  public:
    GraphingModel(View* v, Controller* c, FormulaList* f, ScreenInfo* s): Model{v,c,f,s} {}
};

#endif
