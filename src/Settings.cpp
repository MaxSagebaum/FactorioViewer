//
// Created by Niemand on 4/12/16.
//


#include <Settings.h>
#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>
#include <fstream>
#include <util.hpp>

void Settings::splitList(const std::string& line, std::vector<std::string>& items) {
  size_t lastDelim = 0;
  size_t delim;
  while(std::string::npos != (delim = line.find(':'))) {
    items.push_back(line.substr(lastDelim, delim));

    lastDelim = delim + 1;
  }

  if(0 != line.size()) {
    items.push_back(line.substr(lastDelim));
  }
}

template<typename Type>
void Settings::handleEntry(const std::string& targetName, const std::string& name, const std::string& value, Type& targetValue) {
  if(0 == targetName.compare(name)) {
    targetValue = parseType<Type>(value);
  }
}

template<typename Type>
void Settings::handleEntryList(const std::string& targetName, const std::string& name, const std::string& value, std::vector<Type>& targetValues) {
  if(0 == targetName.compare(name)) {
    std::vector<std::string> items;
    splitList(value, items);

    for(auto&& item : items) {
      targetValues.push_back(parseType<Type>(item));

    }
  }
}

void Settings::readIni() {
  std::ifstream in(INI_NAME);
  if(!in.bad()) {
    std::string line;
    while(getline(in, line)) {
      size_t delim = line.find('=');

      if(std::string::npos == delim) {
        std::string name = line.substr(0, delim);
        std::string value = line.substr(delim + 1);

        handleEntryList(INI_DIRECTORIES, name, value, this->readDirectories);
      }
    }
  }
}

template<typename Type>
void Settings::outputEntryList(std::ofstream& out, const std::string& name, const std::vector<Type>& values) {
  out << name << "=";
  for(int i = 0; i < values.size(); ++i) {
    if(0 != i) {
      out << ":";
    }
    out << values[i];
  }
  out << std::endl;
}

void Settings::writeIni() {
  std::ofstream out(INI_NAME);
  if(!out.bad()) {

    outputEntryList(out, INI_DIRECTORIES, this->readDirectories);
  }
}

bool Settings::parseCommandLine(int nargs, const char **args) {
  bool success = true;
  try {

    TCLAP::CmdLine cmd("Factorio production drawer", ' ', "alpha");

    TCLAP::SwitchArg dotOutputArg("d", "dot", "Switch on the dot graph formating instead of the ascii formatting.", cmd, false);
    TCLAP::ValueArg<double> speedArg("s", "speed", "The construction speed multiplier for the fabrication. [Default: 1.0]  E.g.: Assembling Machine 1 = 0.5, Assembling Machine 2 = 0.75, Assembling Machine 3 = 1.25", false, 1.0, "speed factor", cmd);
    TCLAP::MultiArg<double> unitsArg("u", "units", "The number of units that the recipe should produce per minute. For each recipe a separate amount can be specified. The last unit is used for the remaining recipes. [Default: 1.0]", false, "Units per minute", cmd);
    TCLAP::UnlabeledMultiArg<std::string> recipesArg("recipe", "The recipes for which the ingredients are displayed. The name can either be the internal factorio name or the natural language description.", true, "recipe", cmd);

    TCLAP::MultiArg<std::string> dirArg("", "dir", "A directory that is searched for lua files and these files are then parsed for recipes. "
      "These directories are also read from the ini file in the current folder. The directories will be updated in the ini file when the program finishes."
      "[Default: $HOME/.local/share/Steam/steamapps/common/Factorio/data/base/prototypes/recipe]", false, "lua-dir", cmd);

    TCLAP::MultiArg<std::string> baseArg("b", "base", "Treat the item as a base item and stop the breakdown at this item.", false, "base", cmd);
    TCLAP::SwitchArg iniReadArg("", "no-ini-read", "Do not read the ini file.", cmd, false);
    TCLAP::SwitchArg iniWriteArg("", "no-ini-write", "Do not write the ini file.", cmd, false);

    cmd.parse( nargs, args);

    if(!iniReadArg.isSet()) {
      readIni();
    }

    dotOutput = dotOutputArg.isSet();
    recipes = recipesArg.getValue();
    units = unitsArg.getValue();
    if(0 == units.size()) {
      units.push_back(1.0);
    }
    speed = speedArg.getValue();

    readDirectories = dirArg.getValue();
    if(0 == readDirectories.size()) {
      const char *homeDir;
      if ((homeDir = getenv("HOME")) == NULL) {
        std::cerr << "Could not get the home directory." << std::endl;
      }

      std::string factorioDir = homeDir;
      factorioDir += "/.local/share/Steam/steamapps/common/Factorio/data/base/prototypes/recipe";
      readDirectories.push_back(factorioDir);
    }

    baseComponents = baseArg.getValue();
    baseComponents.push_back("iron-plate");
    baseComponents.push_back("copper-copper");

    if(!iniWriteArg.isSet()) {
      writeIni();
    }

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for argument " << e.argId() << std::endl;
    success = false;
  }

  return success;
}