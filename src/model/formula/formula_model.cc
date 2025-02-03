#include "formula_model.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using std::vector,std::string, std::variant;
using std::ofstream, std::ifstream;

// Processes the commands specific to the Formula Window.
// Returns true if the command was successfully processed, false otherwise.
bool FormulaModel::processCommandSpecific(vector<string> cmdWords) {
  int wordLen = cmdWords.size();
  if (wordLen == 2 && (cmdWords[0] == "select" || cmdWords[0] == "view")){
    int index;
    try{ 
      index = std::stoi(cmdWords[1]);
    } catch(...){ return false; }
    if (index < 1 || index > 99){
      displayCommandError("Index not in range 1-99");
      return true;
    }
    displayFormulas(index, cmdWords[0] == "view");
    selectedFormula = index;
    if (cmdWords[0] == "select"){
      commandMode = false;
      formulaCursorIndex = stringSet[selectedFormula].length();
    } 
    return true;
  } else if (wordLen == 2 && cmdWords[0] == "hide"){
    int index;
    try{ 
      index = std::stoi(cmdWords[1]);
    } catch(...){ return false; }
    if (index < 1 || index > 99){
      displayCommandError("Index not in range 1-99");
      return true;
    }
    formulas->setColour(index, NOCOLOUR);
    onColourChange(index);
    return true;
  } else if (wordLen == 1 && cmdWords[0] == "clear"){
    stringSet = vector<string>(100,"");
    formulas->clear();
    displayFormulas(1);
    displayCommandMessage("Formulas cleared");
    return true;
  } else if (wordLen == 2 && cmdWords[0] == "saveas"){
    if (saveToFile(cmdWords[1])){
      currentFileName = cmdWords[1];
      displayCommandMessage("Successful save");
    } else{
      currentFileName = "";
      displayCommandError("Unable to save to file");
    }
    return true;
  } else if (wordLen == 1 && cmdWords[0] == "save"){
    if (currentFileName == ""){
      displayCommandError("No file on record");
    } else{
      if (saveToFile(currentFileName)) displayCommandMessage("Successful save");
      else{
        currentFileName = "";
        displayCommandError("Unable to save to file");
      }
    }
    return true;
  } else if (wordLen == 2 && cmdWords[0] == "load"){
    if (loadFromFile(cmdWords[1])){
      currentFileName = cmdWords[1];
      displayCommandMessage("Successful load");
      displayFormulas(selectedFormula, true);
    } else{
      currentFileName = "";
      displayCommandError("Unable to load from file");
    }
    return true;
  } else if (wordLen == 1 && cmdWords[0] == "new"){
    currentFileName = "";
    displayCommandMessage("File history erased");
    return true;
  }
  return false;
}

// The logic for when the user is not in command mode
// For the formula window, this means they are editing a formula.
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
  displayFormulas(1, true);
}

// Refreshes the formula display when the colour of a formula changes.
void FormulaModel::onColourChange(int index){
  displayFormulas(selectedFormula, true);
}

// Refreshes the formula display when screen size changes.
void FormulaModel::onScreenSizeChange() {
  displayFormulas(selectedFormula);
}

// Updates the view to display the stored formulas.
// If extras is true, it will also display error messages and colour the formulas.
// Display formulas starting from the "startInd" index.
void FormulaModel::displayFormulas(int startInd, bool extras){
  int currentLine = 0;
  for (int ind = startInd; currentLine < maxRow - 3 && ind < 100; ind++){
    currentLine += displaySingleFormula(currentLine, ind, extras);
    if (extras){ // Include error messages and colours
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

// Returns the number of lines it used to display the formula
int FormulaModel::displaySingleFormula(int line, int index, bool useColouring){
  int currentLine = line;
  vector<Colour> colourScheme((index < 10 ? 2 : 3), (formulas->getColour(index) == NOCOLOUR ? BLACK : formulas->getColour(index)));
  while (colourScheme.size() < FORMULA_BUFFER) colourScheme.push_back(WHITE);
  if (useColouring){
    vector<Colour> colouring = formulas->getFormulaColouring(index);
    if (!colouring.empty()){
      int colouringInd = 0;
      for (auto c : stringSet[index]){
        if (isspace(c)) colourScheme.push_back(WHITE);
        else colourScheme.push_back(colouring[colouringInd++]);
      }
    }
  }
  view->updateRow(currentLine, std::to_string(index) + (index < 10 ? ".  " : ". ") + stringSet[index], colourScheme);
  currentLine++;
  int printedIndex = maxCol - FORMULA_BUFFER, maxInd = stringSet[index].length();
  while (printedIndex < maxInd && currentLine < maxRow-3){
    if (useColouring && colourScheme.size() >= maxCol - FORMULA_BUFFER) colourScheme.erase(colourScheme.begin()+FORMULA_BUFFER, colourScheme.begin()+maxCol);
    view->updateRow(currentLine, string(FORMULA_BUFFER,' ') + stringSet[index].substr(printedIndex), colourScheme);
    printedIndex += maxCol - FORMULA_BUFFER;
    currentLine++;
  }
  return currentLine - line;
}

// Saves the current formula set to a file.
// Returns true if the save was successful, false otherwise.
bool FormulaModel::saveToFile(const string& fileName){
  ofstream saveFile;
  saveFile.open(fileName);
  if (!saveFile.is_open()) return false;
  for (int ind = 1; ind <= 99; ind++){
    if (stringSet[ind] != ""){
      saveFile << ind << ' ' 
               << getStringFromColour(formulas->getColour(ind)) << ' '
               << stringSet[ind] << '\n';
    }
  }
  saveFile.close();
  return true;
}

// Loads formulas from a provided file.
// Returns true if the load was successful, false otherwise.
bool FormulaModel::loadFromFile(const std::string& fileName){
  ifstream loadFile;
  loadFile.open(fileName);
  if (!loadFile.is_open()) return false;

  // Resets state
  stringSet = vector<string>(100,"");
  formulas->clear();

  string line;
  int lineLen;
  while (getline(loadFile, line)){
    lineLen = line.length();
    int lineInd = 0;
    while (lineInd <lineLen && line[lineInd] != ' ') ++lineInd;
    if (lineInd == lineLen) continue;
    int index = -1;
    try { index = stoi(line.substr(0, lineInd)); }
    catch(...) {}
    if (index < 1 || index > 99) continue;
    while (lineInd < lineLen && line[lineInd] == ' ') ++lineInd;
    if (lineInd == lineLen) continue;
    int colourInd = lineInd;
    while (lineInd < lineLen && line[lineInd] != ' ') ++lineInd;
    if (lineInd == lineLen) continue;
    Colour newColour = getColourFromString(line.substr(colourInd, lineInd-colourInd));
    if (newColour == BLACK) continue;
    while (lineInd < lineLen && line[lineInd] == ' ') ++lineInd;
    if (lineInd == lineLen) continue;
    int formulaStart = lineInd;
    while (lineInd < lineLen && line[lineInd] != '\n') ++lineInd;
    stringSet[index] = line.substr(formulaStart, lineInd-formulaStart);
    formulas->updateFormula(index, stringSet[index]);
    formulas->setColour(index, newColour);
  }
  loadFile.close();
  return true;
}
