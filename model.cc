#include "model.h"

using std::pair;

void Model::updateScreenDimensions(){
  pair<int,int> newDims = controller->getScreenSize();
  if (newDims.first != maxRow || newDims.second != maxCol){
    /* Refresh screen with new dimensions goes here */
    maxRow = newDims.first;
    maxCol = newDims.second;
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
          if (currentCommand.length() >= maxCol){
            view->updateRow(commandRow, currentCommand.substr(currentCommand.length()-maxCol, maxCol));
          } else view->updateRow(commandRow, currentCommand);
          view->moveCursor(commandRow, commandCursorIndex);
        }
        break;
      case ENTER:
        processCommandDefault();
        currentCommand = "";
        commandCursorIndex = 0;
        break;
      case LEFTARROW:
        if (commandCursorIndex > 0) --commandCursorIndex;
        view->moveCursor(commandRow, commandCursorIndex);
        break;
      case RIGHTARROW:
        if (commandCursorIndex < maxCol && commandCursorIndex < currentCommand.length()){
          ++commandCursorIndex;
        }
        view->moveCursor(commandRow, commandCursorIndex);
        break;
      default: break;
    }
  } else{
    char newChar = std::get<char>(input);
    if (commandCursorIndex == currentCommand.length()){
      currentCommand += newChar;
      if (commandCursorIndex == maxCol-1){
        view->updateRow(commandRow, currentCommand.substr(currentCommand.length()-maxCol, maxCol));
      } else{
        view->updatePlace(commandRow, commandCursorIndex, newChar);
        commandCursorIndex++;
      }
    } else{
      currentCommand[commandCursorIndex] = newChar;
      view->updatePlace(commandRow, commandCursorIndex, newChar);
      commandCursorIndex++;
    }
  }
}

void Model::processCommandDefault(){
  if (currentCommand == "quit" || currentCommand == "q") exitStatus = QUIT;
  else if (currentCommand == "switch") exitStatus = SWITCH;
  else {
    processCommandSpecific();
  }
}

ExitStatus Model::runModel(){
  commandCursorIndex = 0;
  currentCommand = "";

  updateScreenDimensions();
  view->wipe();
  view->moveCursor(maxRow-2, commandCursorIndex);
  while (exitStatus == NOSTATUS){
    updateScreenDimensions();
    if (commandMode){
      updateCommand(controller->getInput());
    } else exitStatus = QUIT;
    view->loadScreen();
  }
  return exitStatus;
}
