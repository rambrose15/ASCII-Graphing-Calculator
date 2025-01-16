#ifndef __GRAPHING_MODEL_H__
#define __GRAPHING_MODEL_H__

#include "../model.h"
#include <set>
#include <chrono>
#include <ctime>

class GraphingModel : public Model{
  GraphPackage gp; // Stores the data on the graphs of the functions
  std::vector<BigRational> vLineSet, hLineSet; // Store the h&v lines
  std::set<int> hidden;
  std::set<int> playing;
  int displayValueInd;
  BigRational displayValueInput;

  inline static const std::string defaultZoom = "2.0";
  BigRational currentZoom;
  bool zoomSelectMode, zoomingIn;
  bool traceMode;
  int traceIndex;
  int cursorPosX, cursorPosY;

  std::chrono::system_clock::time_point startTime;
  inline static const double UPDATE_TIME = 1.2;
  
  void initializeSpecific() override;
  void graphFunctions();
  void graphLines(std::vector<std::string>& graphScreen);
  void updateGP();
  void parameterUpdate(const std::set<int>& params);

  bool processCommandSpecific(std::vector<std::string> cmdWords) override;
  void runInsideCommand() override;
  void runOutsideCommand() override;
  
  void zoomSelect(KeyPress key);
  std::pair<BigRational,BigRational> ComputeZoom(const BigRational& lBound, const BigRational& uBound, double pos);
  void trace(KeyPress key);

  void onColourChange(int index) override;
  void onScreenSizeChange() override;
  void onScreenCoordsChange() override;
  void onCommandExecute() override;

  public:
    GraphingModel(View* v, Controller* c, FormulaList* f, ScreenInfo* s): Model{v,c,f,s} {}
};

#endif
