#include "graphingModel.h"

using std::vector, std::pair, std::string;

void GraphingModel::initializeSpecific() {
  if (vLineSet.empty()) vLineSet.push_back(BigRational("0"));
  if (hLineSet.empty()) hLineSet.push_back(BigRational("0"));
  hidden.clear();
  playing.clear();
  startTime = std::chrono::system_clock::now();
  updateGP();
  graphFunctions();
}

bool GraphingModel::processCommandSpecific(vector<string> cmdWords){
  if (cmdWords.size() == 2 && cmdWords[0] == "vline"){
    try{ vLineSet.push_back(BigRational(cmdWords[1])); } 
    catch(...){ return false; }
    graphFunctions();
    return true;
  } else if (cmdWords.size() == 2 && cmdWords[0] == "hline"){
    try{ hLineSet.push_back(BigRational(cmdWords[1])); } 
    catch(...){ return false; }
    graphFunctions();
    return true;
  } else if (cmdWords.size() == 1 && cmdWords[0] == "removelines"){
    hLineSet.erase(hLineSet.begin()+1, hLineSet.end());
    vLineSet.erase(vLineSet.begin()+1, vLineSet.end());
    graphFunctions();
    return true;
  } else if (cmdWords.size() == 2 && (cmdWords[0] == "hide" || cmdWords[0] == "show")){
    int index;
    try{ index = std::stoi(cmdWords[1]); }
    catch(...) { return false; }
    if (index < 1 || index > 99){
      displayCommandError("Index not in range 1-99");
      return true;
    }
    if (cmdWords[0] == "hide") hidden.insert(index);
    else hidden.erase(index);
    graphFunctions();
    return true;
  } else if (cmdWords.size() >= 1 && (cmdWords[0] == "play" || cmdWords[0] == "stop")){
    int wordLen = cmdWords.size();
    if (wordLen == 1){
      if (cmdWords[0] == "play") {
        for (int i = 1; i < 100; i++) if (formulas->isParameter(i)) playing.insert(i);
      } else playing.clear();
    }
    bool someFailed = false;
    for (int i = 1; i < wordLen; i++){
      int index = -1;
      try{ index = std::stoi(cmdWords[i]); }
      catch(...) { someFailed = true; }
      if (index < 1 || index > 99) someFailed = true;
      else{
        if (cmdWords[0] == "play" && formulas->isParameter(index)) playing.insert(index);
        else if (cmdWords[0] == "stop" && playing.count(index)) playing.erase(index);
      }
    }
    if (someFailed) displayCommandError("Some of the provided indices were invalid");
    return true;
  }
  return false;
}

void GraphingModel::runInsideCommand() {
  if (!playing.empty()){
    std::chrono::duration<double> timeElasped = std::chrono::system_clock::now() - startTime;
    if (timeElasped.count() > UPDATE_TIME){
      formulas->updateParameterized(maxRow-2, maxCol, screenInfo->screenDimXL, screenInfo->screenDimXR,
      screenInfo->screenDimYL, screenInfo->screenDimYR, gp, playing, 1);
      graphFunctions();
      startTime = std::chrono::system_clock::now();
    }
  } else startTime = std::chrono::system_clock::now();
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
  
  graphLines(graphScreen); // Graphs the h&v lines, which include the axes

  // Get the character for the functions of x
  for (int ind = 0, n = gp.xFuncIndices.size(); ind < n; ++ind){
    for (int pos = 0; pos < maxCol; ++pos){
      if (gp.xStrings[ind][pos] == ' ') continue;
      if (hidden.count(gp.xFuncIndices[ind])) continue;
      if (formulas->getColour(gp.xFuncIndices[ind]) == NOCOLOUR) continue;
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
      if (formulas->getColour(gp.yFuncIndices[ind]) == NOCOLOUR) continue;
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

void GraphingModel::graphLines(vector<string>& graphScreen){
  BigRational squareSize = (screenInfo->screenDimXR - screenInfo->screenDimXL) / BigRational(std::to_string(maxCol));
  for (auto line : vLineSet){
    if (screenInfo->screenDimXR < line || line < screenInfo->screenDimXL) continue;
    int charInd = 0;
    for (BigRational pos = screenInfo->screenDimXL+squareSize; pos < line; pos = pos+squareSize) charInd++;
    for (int i = 0; i < maxRow-2; i++) graphScreen[i][charInd] = '|';
  }
  squareSize = (screenInfo->screenDimYR - screenInfo->screenDimYL) / BigRational(std::to_string(maxRow));
  for (auto line : hLineSet){
    if (screenInfo->screenDimYR < line || line < screenInfo->screenDimYL) continue;
    int charInd = 0;
    for (BigRational pos = screenInfo->screenDimYL+squareSize; pos < line; pos = pos+squareSize) charInd++;
    for (int i = 0; i < maxCol; i++) {
      if (graphScreen[charInd][i] == '|') graphScreen[charInd][i] = '+';
      else graphScreen[charInd][i] = '-';
    }
  }
}
