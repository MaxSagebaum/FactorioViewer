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
  while (std::string::npos != (delim = line.find(';', lastDelim))) {
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

      if(std::string::npos != delim) {
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
  for(size_t i = 0; i < values.size(); ++i) {
    if(0 != i) {
      out << ";";
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

    TCLAP::SwitchArg dotOutputArg("d", "dot", "Switch on the dot graph formatting instead of the ascii formatting.", cmd, false);
    TCLAP::SwitchArg dotGatherArg("g", "dotGather", "Gather the units when dot formatting is enabled.", cmd, false);
    TCLAP::ValueArg<double> factorySpeedArg("s", "sSpeed", "The construction speed multiplier for the factory fabrication. [Default: 1.0]  E.g.: Assembling Machine 1 = 0.5, Assembling Machine 2 = 0.75, Assembling Machine 3 = 1.25", false, 1.0, "factory speed factor", cmd);
    TCLAP::ValueArg<double> chemicalSpeedArg("c", "cSpeed", "The construction speed multiplier for the chemical fabrication. [Default: 1.25]  E.g.: Chemical plant = 1.25", false, 1.25, "chemical speed factor", cmd);
    TCLAP::ValueArg<double> furnaceSpeedArg("f", "fSpeed", "The construction speed multiplier for the furnace fabrication. [Default: 2.0]  E.g.: Stone furnace = 1.0, Steel furnace = 2.0, Electric furnace = 2.0", false, 2.0, "furnace speed factor", cmd);
    TCLAP::ValueArg<double> oilSpeedArg("", "oSpeed", "The construction speed multiplier for the oil fabrication. [Default: 1.0]  E.g.: Oil refinery = 1.0", false, 1.0, "oil speed factor", cmd);
    TCLAP::ValueArg<double> rocketSpeedArg("", "rSpeed", "The construction speed multiplier for the rocket fabrication. [Default: 1.0]  E.g.: Rocket silo = 1.0", false, 1.0, "rocket speed factor", cmd);
    TCLAP::ValueArg<double> centriSpeedArg("", "centriSpeed", "The construction speed multiplier for the centrifugal fabrication. [Default: 0.75]  E.g.: Centrifuge = 0.75", false, 0.75, "centrifuge speed factor", cmd);
    TCLAP::MultiArg<double> unitsArg("u", "units", "The number of units that the recipe should produce per minute. For each recipe a separate amount can be specified. The last unit is used for the remaining recipes. [Default: 1.0]", false, "Units per minute", cmd);

    TCLAP::ValueArg<std::string> oilRecipeArg("", "oilRecipe", "The recipe for oil processing. [Default: advanced-oil-processing]  E.g.: basic-oil-processing, advanced-oil-processing, coal-liquefaction", false, "advanced-oil-processing", "oil recipe", cmd);

    TCLAP::UnlabeledMultiArg<std::string> recipesArg("recipe", "The recipes for which the ingredients are displayed. The name can either be the internal Factorio name or the natural language description.", true, "recipe");

    TCLAP::MultiArg<std::string> dirArg("", "dir", "A directory that is searched for lua files and these files are then parsed for recipes. "
      "These directories are also read from the ini file in the current folder. The directories will be updated in the ini file when the program finishes."
      "[Default: $HOME/.local/share/Steam/steamapps/common/Factorio/data/base/prototypes/recipe]", false, "lua-dir", cmd);

    TCLAP::MultiArg<std::string> baseArg("b", "base", "Treat the item as a base item and stop the breakdown at this item.", false, "base", cmd);
    TCLAP::MultiArg<std::string> totalArg("t", "total", "Compute a total for the item.", false, "total", cmd);
    TCLAP::SwitchArg iniReadArg("", "no-ini-read", "Do not read the ini file.", cmd, false);
    TCLAP::SwitchArg iniWriteArg("", "no-ini-write", "Do not write the ini file.", cmd, false);
    TCLAP::SwitchArg totalAllArg("a", "all", "Show a total for all items. [Default: Total is only shown for the base items.].", cmd, false);
    TCLAP::SwitchArg useExpensiveArg("e", "expensive", "Use the expensive version of the required items.", cmd, false);

    TCLAP::SwitchArg listArg("l", "list", "List all recipes that are available.", false);

    cmd.xorAdd(recipesArg, listArg);
    cmd.parse( nargs, args);

    if(!iniReadArg.isSet()) {
      readIni();
    }

    dotOutput = dotOutputArg.isSet();
    unifyDot = dotGatherArg.isSet();
    totalAll = totalAllArg.isSet();
    useExpensive = useExpensiveArg.isSet();
    list = listArg.isSet();
    recipes = recipesArg.getValue();
    units = unitsArg.getValue();
    if(0 == units.size()) {
      units.push_back(1.0);
    }
    factorySpeed = factorySpeedArg.getValue();
    chemicalSpeed = chemicalSpeedArg.getValue();
    furnanceSpeed = furnaceSpeedArg.getValue();
    oilSpeed = oilSpeedArg.getValue();
    rocketSpeed = rocketSpeedArg.getValue();
    centriSpeed = centriSpeedArg.getValue();

    oilRecipe = oilRecipeArg.getValue();

    std::vector<std::string> addionalDirs = dirArg.getValue();
    for (auto&& item : addionalDirs) {
      readDirectories.push_back(item);
    }
    if(0 == readDirectories.size()) {
      const char *homeDirC;
	    std::string homeDir = "";

#ifdef _WINDOWS
      if ((homeDirC = getenv("HOMEDRIVE")) == NULL) {
        std::cerr << "Could not get the home drive." << std::endl;
        exit(-1);
      }
      homeDir = homeDirC;
      if ((homeDirC = getenv("HOMEPATH")) == NULL) {
        std::cerr << "Could not get the home path." << std::endl;
        exit(-1);
      }
      homeDir += homeDirC;
#else
      if ((homeDirC = getenv("HOME")) == NULL) {
        std::cerr << "Could not get the home directory." << std::endl;
        exit(-1);
	    } else {
          homeDir = homeDirC;
	    }
#endif

      std::string factorioDir = homeDir;
      factorioDir += "/.local/share/Steam/steamapps/common/Factorio/data/base/prototypes/recipe";
      readDirectories.push_back(factorioDir);
    }

    baseComponents = baseArg.getValue();

    totalComponents = totalArg.getValue();
    // add all base components that the use defines to the total components
    for(auto&& base : baseComponents) {
      totalComponents.push_back(base);
    }

    // add default base components
    baseComponents.push_back("crude-oil");
    baseComponents.push_back("iron-ore");
    baseComponents.push_back("copper-ore");
    baseComponents.push_back("iron-plate");
    baseComponents.push_back("copper-plate");
    baseComponents.push_back("water");
    baseComponents.push_back("coal");
    baseComponents.push_back("alien-artifact");

    // add default total components
    totalComponents.push_back("iron-plate");
    totalComponents.push_back("copper-plate");
    totalComponents.push_back("heavy-oil");
    totalComponents.push_back("light-oil");
    totalComponents.push_back("petroleum-gas");
    totalComponents.push_back("water");
    totalComponents.push_back("coal");
    totalComponents.push_back("alien-artifact");
    if(!iniWriteArg.isSet()) {
      writeIni();
    }

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for argument " << e.argId() << std::endl;
    success = false;
  }

  return success;
}
