#include "formulaModel.h"
#include <vector>
#include <string>

using std::vector,std::string, std::variant;

bool FormulaModel::processCommandSpecific(vector<string> cmdWords) {
  if (cmdWords.size() == 2 && cmdWords[0] == "select"){
    int index;
    try{ 
      index = std::stoi(cmdWords[1]);
    } catch(...){ return false; }
    if (index < 1 || index > 99){
      displayCommandError("Index not in range 1-99");
      return true;
    }
    displayFormulas(index);
    selectedFormula = index;
    commandMode = false;
    formulaCursorIndex = stringSet[selectedFormula].length();
    return true;
  } else if (cmdWords.size() == 2 && cmdWords[0] == "view"){
    int index;
    try{ index = std::stoi(cmdWords[1]); }
    catch(...) { return false; }
    if (index < 1 || index > 99){
      displayCommandError("Index not in range 1-99");
      return true;
    }
    displayFormulas(index, true);
    selectedFormula = index;
    return true;
  }
  return false;
}

void FormulaModel::runOutsideCommand(){
  variant<char,KeyPress> input = controller->getInput();
  if (std::holds_alternative<KeyPress>(input)){
    KeyPress key = std::get<KeyPress>(input);
    switch(key){
      case (BACKSPACE):
        if (formulaCursorIndex != 0){
          formulaCursorIndex--;
          stringSet[selectedFormula].erase(stringSet[selectedFormula].begin()+formulaCursorIndex);
          // If the change alters the number of lines
          if (stringSet[selectedFormula].length() > 0 && 
              stringSet[selectedFormula].length() % (maxCol - FORMULA_BUFFER) == 0){
            displayFormulas(selectedFormula);
          } 
          // If we're not at the very back of the string
          else if (formulaCursorIndex != stringSet[selectedFormula].length()){
            displaySingleFormula(0, selectedFormula);
          } else{
            view->updatePlace(formulaCursorIndex / (maxCol - FORMULA_BUFFER), 
              FORMULA_BUFFER + (formulaCursorIndex % (maxCol - FORMULA_BUFFER)), ' ');
          }
        }
        break;
      case (ESC): case(ENTER):
        formulas->updateFormula(selectedFormula, stringSet[selectedFormula]);
        displayFormulas(selectedFormula, true);
        commandMode = true;
        break;
      case(LEFTARROW):
        if (formulaCursorIndex > 0) --formulaCursorIndex;
        break;
      case(RIGHTARROW):
        if (formulaCursorIndex < stringSet[selectedFormula].length()) ++formulaCursorIndex;
        break;
      default: break;
    }
  } 
  // Input is simply a character
  else if (std::holds_alternative<char>(input)){
    char newChar = std::get<char>(input);
    stringSet[selectedFormula].insert(stringSet[selectedFormula].begin()+formulaCursorIndex, newChar);
    formulaCursorIndex++;
    // If the new character changes the number of lines we take up
    if (stringSet[selectedFormula].length() != 1 && 
        stringSet[selectedFormula].length() % (maxCol - FORMULA_BUFFER) == 1){
        displayFormulas(selectedFormula);
    }
    // If we're not at the very end of the string
    else if (formulaCursorIndex != stringSet[selectedFormula].length()){
      displaySingleFormula(0, selectedFormula);
    } else{
      view->updatePlace(formulaCursorIndex / (maxCol - FORMULA_BUFFER), 
        FORMULA_BUFFER-1 + (formulaCursorIndex % (maxCol - FORMULA_BUFFER)), newChar);
    }
  }
  view->moveCursor(formulaCursorIndex / (maxCol - FORMULA_BUFFER), 
                  FORMULA_BUFFER + (formulaCursorIndex % (maxCol - FORMULA_BUFFER)));
}

void FormulaModel::initializeSpecific() {
  selectedFormula = 1;
  //preLoad();
  displayFormulas(1);
}

void FormulaModel::onColourChange(int index){
  displayFormulas(selectedFormula);
}

void FormulaModel::onScreenSizeChange() {
  displayFormulas(selectedFormula);
}

void FormulaModel::preLoad(){ // Currently for debugging purposes
  vector<string> strs = {"f(x) = x^2-8"};
  for (int ind = 0, n = strs.size(); ind < n; ind++){
    formulas->updateFormula(ind+1, strs[ind]);
  }
}

void FormulaModel::displayFormulas(int startInd, bool includeErrors){
  int currentLine = 0;
  for (int ind = startInd; currentLine < maxRow - 3 && ind < 100; ind++){
    currentLine += displaySingleFormula(currentLine, ind);
    if (includeErrors){
      if (formulas->getErrorStatus(ind) != NONE){
        string errorString = formulaErrorString(formulas->getErrorStatus(ind));
        int maxInd = errorString.length();
        for (int printedIndex = 0; printedIndex < maxInd && currentLine < maxRow - 3; printedIndex += maxCol - FORMULA_BUFFER){
          view->updateRow(currentLine, errorString.substr(printedIndex), vector<Colour>(maxInd, RED));
          ++currentLine;
        }
      }
    }
  }
  while (currentLine <= maxRow-3) view->updateRow(currentLine++, "");
}

// Returns the number of lines it used
int FormulaModel::displaySingleFormula(int line, int index){
  int currentLine = line;
  view->updateRow(currentLine, std::to_string(index) + (index < 10 ? ".  " : ". ") + stringSet[index], 
                  vector<Colour>(FORMULA_BUFFER, formulas->getColour(index)));
  currentLine++;
  int printedIndex = maxCol - FORMULA_BUFFER, maxInd = stringSet[index].length();
  while (printedIndex < maxInd && currentLine < maxRow-3){
    view->updateRow(currentLine, string(FORMULA_BUFFER,' ') + stringSet[index].substr(printedIndex));
    printedIndex += maxCol - FORMULA_BUFFER;
    currentLine++;
  }
  return currentLine - line;
}
