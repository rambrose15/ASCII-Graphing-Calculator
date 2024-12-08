#include "graphingModel.h"

using std::vector, std::pair, std::string;

void GraphingModel::initializeSpecific() {
  updateGP();
  graphFunctions();
}

bool GraphingModel::processCommandSpecific(vector<string> cmdWords){
  // Graphing window specfic commands
  return false;
}

void GraphingModel::runInsideCommand() {

}

void GraphingModel::runOutsideCommand() {}

void GraphingModel::onColourChange(int index){
  graphFunctions();
}

void GraphingModel::onScreenSizeChange() {
  updateGP();
  graphFunctions();
}

void GraphingModel::graphFunctions(){
  vector<string> graphScreen(maxRow-2, string(maxCol, ' '));
  vector<vector<Colour>> colourScheme(maxRow-2, vector<Colour>(maxCol, WHITE));
  
  // Get the character for the functions of x
  for (int ind = 0, n = gp.xFuncIndices.size(); ind < n; ++ind){
    for (int pos = 0; pos < maxCol; ++pos){
      if (gp.xStrings[ind][pos] == ' ') continue;
      if (graphScreen[gp.xFuncPositions[ind][pos]][pos] != ' '){
        graphScreen[gp.xFuncPositions[ind][pos]][pos] = '#'; 
        colourScheme[gp.xFuncPositions[ind][pos]][pos] = BLACK;
      } else{
        graphScreen[gp.xFuncPositions[ind][pos]][pos] = gp.xStrings[ind][pos];
        colourScheme[gp.xFuncPositions[ind][pos]][pos] = formulas->getColour(gp.xFuncIndices[ind]);
      }
    }
  }
  // Get the characters for the functions of y
  for (int ind = 0, n = gp.yFuncIndices.size(); ind < n; ++ind){
    for (int pos = 0; pos < maxRow-2; ++pos){
      if (gp.yStrings[ind][pos] == ' ') continue;
      if (graphScreen[pos][gp.yFuncPositions[ind][pos]] != ' '){
        graphScreen[pos][gp.yFuncPositions[ind][pos]] = '#';
        colourScheme[pos][gp.yFuncPositions[ind][pos]] = BLACK;
      } else{
        graphScreen[pos][gp.yFuncPositions[ind][pos]] = gp.yStrings[ind][pos];
        colourScheme[pos][gp.yFuncPositions[ind][pos]] = formulas->getColour(gp.yFuncIndices[ind]);
      }
    }
  }
  for (int ind = 0; ind < maxRow-2; ++ind){
    view->updateRow(maxRow-3-ind, graphScreen[ind], colourScheme[ind]);
  }
}

void GraphingModel::updateGP(){
  gp = formulas->getGraphs(maxRow-2, maxCol, 
    screenInfo->screenDimXL, screenInfo->screenDimXR, screenInfo->screenDimYL, screenInfo->screenDimYR);
}
