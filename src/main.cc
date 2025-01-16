#include "model/graphing_calculator.h"
#include <string>

int main(int argc, char* args[]){
  GraphingCalculator gc{};
  if (argc == 2) gc.loadBeforeRun(std::string(args[1]));
  gc.run();
}
