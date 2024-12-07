#include "model.h"

#include <sstream>

using std::pair, std::string, std::vector;

void Model::updateScreenDimensions(){
  pair<int,int> newDims = controller->getScreenSize();
  if (newDims.first != maxRow || newDims.second != maxCol){
    maxRow = newDims.first;
    maxCol = newDims.second;

    // Reset the command
    updateCommand(NOPRESS);
    
    // Reset command message
    if (commandError) view->updateRow(maxRow-1, commandMessage, vector<Colour>(commandMessage.length(), RED));
    else view->updateRow(maxRow-1, commandMessage);

    onScreenSizeChange();
  }
}

void Model::updateCommand(std::variant<char,KeyPress> input){
  int commandRow = maxRow-2;
  if (std::holds_alternative<KeyPress>(input)){
    KeyPress key = std::get<KeyPress>(input);
    switch(key){
      case BACKSPACE:
        if (commandCursorIndex > 0){
          currentCommand.erase(commandCursorIndex-1, 1);
          commandCursorIndex--;
          // Case that the tail of the command is not fully on-screen
          if (maxCol - commandCursorPosition >= currentCommand.length() - commandCursorIndex){
            if (commandCursorPosition > 0) --commandCursorPosition;
            //view->updateRow(commandRow, currentCommand.substr(commandCursorIndex-commandCursorPosition, maxCol));
          } 
          // Case that the front of the command is not fully on-screen
          else if (commandCursorIndex > commandCursorPosition){
            //view->updateRow(commandRow, currentCommand.substr(commandCursorIndex-commandCursorPosition));
          } 
          else{ // The normal case
            --commandCursorPosition;
            //view->updateRow(commandRow, currentCommand);
          }
        }
        break;
      case ENTER:
        processCommandDefault();
        currentCommand = " ";
        commandCursorIndex = 0;
        commandCursorPosition = 0;
        break;
      case LEFTARROW:
        /*if (commandCursorPosition == 0 && commandCursorIndex > 0){
          view->updateRow(commandRow, currentCommand.substr(commandCursorIndex-1, maxCol));
        }*/
        if (commandCursorPosition > 0) --commandCursorPosition;
        if (commandCursorIndex > 0) --commandCursorIndex;
        break;
      case RIGHTARROW:
        if (commandCursorPosition == maxCol-1 && commandCursorIndex < currentCommand.length()-1){
          //view->updateRow(commandRow, currentCommand.substr(commandCursorIndex - maxCol + 2, maxCol));
        }
        if (commandCursorIndex < currentCommand.length()-1){
          ++commandCursorIndex;
          if (commandCursorPosition < maxCol-1) ++commandCursorPosition;
        }
        break;
      default: break;
    }
  } else{
    char newChar = std::get<char>(input);
    currentCommand.insert(currentCommand.begin()+commandCursorIndex, newChar);
    ++commandCursorIndex;
    if (commandCursorPosition < maxCol-1) ++commandCursorPosition;
    //view->updateRow(commandRow, currentCommand.substr(commandCursorIndex-commandCursorPosition));
  }
  if (commandMode) {
    view->updateRow(commandRow, currentCommand.substr(commandCursorIndex - commandCursorPosition));
    view->moveCursor(commandRow, commandCursorPosition);
  }
}

void Model::processCommandDefault(){
  std::stringstream cmdStream(currentCommand);
  vector<string> cmdWords;
  string s;
  while (cmdStream >> s) cmdWords.push_back(s);
  int wordNum = cmdWords.size();

  commandMessage = ""; commandError = false;

  if (wordNum == 1 && (cmdWords[0] == "quit" || cmdWords[0] == "q")) exitStatus = QUIT;
  else if (wordNum == 1 && (cmdWords[0] == "swap" || cmdWords[0] == "s")) exitStatus = SWITCH;
  else if (wordNum == 5 && cmdWords[0] == "screencoords"){
    try{
        BigRational newXL(cmdWords[1]), newXR = cmdWords[2];
        BigRational newYL(cmdWords[3]), newYR = cmdWords[4];
        if (newXL + BigRational("0.001") < newXR && newYL + BigRational("0.001") < newYR){
          screenDimXL = newXL; screenDimXR = newXR;
          screenDimYL = newYL; screenDimYR = newYR;
        } else throw "Incorrect sizes";
    } catch(...){
      displayCommandMessage("Invalid set of screen coordinates");
    }
  } else if (wordNum == 3 && cmdWords[0] == "colour"){
    int index;
    try{ 
      index = std::stoi(cmdWords[1]); 
      if (index < 1 || index > 99){
        displayCommandError("Index not in range 1-99");
      } else{
        Colour newColour = view->getColourFromString(cmdWords[2]);
        if (newColour == BLACK) displayCommandError("Invalid colour name");
        else{
          formulas->setColour(index, newColour);
          onColourChange(index);
        }
      }
    } catch(...) {}
  }
  else {
    if (!processCommandSpecific(cmdWords)){
      displayCommandError("Unrecognized command");
    }
  }
  if (commandError) view->updateRow(maxRow-1, commandMessage, vector<Colour>(commandMessage.length(), RED));
  else view->updateRow(maxRow-1, commandMessage);
}

ExitStatus Model::runModel(){
  // Reset status
  exitStatus = NOSTATUS;

  // Reset command line variables
  commandCursorIndex = 0;
  commandCursorPosition = 0;
  currentCommand = " ";
  commandMessage = "";
  commandError = false;

  // Reset the screen
  updateScreenDimensions();
  view->wipe();
  view->moveCursor(maxRow-2, commandCursorIndex);

  // Calls the intiaization specfic to the particular model
  initializeSpecific();

  // Program loop
  while (exitStatus == NOSTATUS){
    updateScreenDimensions();
    if (commandMode){
      updateCommand(controller->getInput());
      runInsideCommand();
    } else {
      runOutsideCommand();
    }
    view->loadScreen();
  }
  return exitStatus;
}

void Model::displayCommandError(string message){
  commandMessage = message;
  commandError = true;
}

void Model::displayCommandMessage(string message){
  commandMessage = message;
  commandError = false;
}
