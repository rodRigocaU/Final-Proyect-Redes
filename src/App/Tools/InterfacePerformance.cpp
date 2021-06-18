#include "App/Tools/InterfacePerformance.hpp"

using namespace tool;

const std::string getInput(const char* message, const char* COLOR_){
  ConsolePrint(message, COLOR_);
  std::string obj; 
  std::getline(std::cin, obj);
  return obj;
}

void cleanSpaces(std::string& word){
  bool spaces = true;
  int cont(0);
  while (spaces){
    if (word.substr(cont, 1) == " ")
      cont++;
    else
      spaces = false;
  }
  spaces = true;
  int contF = (word.length() - 1);
  while (spaces)
  {
    if (word.substr(contF, 1) == " ")
      contF--;
    else
      spaces = false;
  }
  int dife = word.length() -  contF;
  word = word.substr(cont, word.length() - (dife - 1) - cont);
}

bool readSettingsFile(const char* filePath, std::map<std::string, std::string>& storage){
  std::ifstream settingsFile;
  settingsFile.open(filePath);
  if(settingsFile.is_open()){
    std::string settingsLine, key, value;
    while(std::getline(settingsFile, settingsLine)){
      std::stringstream tokenGroup;
      tokenGroup << settingsLine;
      std::getline(tokenGroup, key, '=');
      std::getline(tokenGroup, value);
      if(value.length()){
        tool::cleanSpaces(key);
        tool::cleanSpaces(value);
        if(storage.find(key) != storage.end())
          storage[key] = value;
      }
    }
    settingsFile.close();
    return true;
  }
  return false;
}