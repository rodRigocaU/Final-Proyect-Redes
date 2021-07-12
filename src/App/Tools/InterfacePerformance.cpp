#include "App/Tools/InterfacePerformance.hpp"

namespace tool{

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

  bool readSettingsFile(const char* filePath, std::map<std::string, std::string>& storage, bool cleanQuots){
    std::ifstream settingsFile;
    settingsFile.open(filePath);
    if(settingsFile.is_open()){
      std::string line, key, value;
      while(std::getline(settingsFile, line)){
        if(line.length() > 1 && (line[0] != DOT_CONF_TOKEN_COMMENT && line[0] != '\n' && line[0] != '\t')){
          std::stringstream tokenGroup;
          tokenGroup << line;
          std::getline(tokenGroup, key, DOT_CONF_TOKEN_SEPARATOR);
          std::getline(tokenGroup, value);
          if(value.length()){
            tool::cleanSpaces(key);
            if(cleanQuots && value.length() >= 3){
              tool::cleanSpaces(value);
              value = value.substr(1, value.length() - 2);
            }
            tool::cleanSpaces(value);
            if(storage.count(key))
              storage[key] = value;
          }
        }
      }
      for(auto& item : storage){
        if(item.second == ""){
          return false;
        }
      }
      settingsFile.close();
      return true;
    }
    return false;
  }

}