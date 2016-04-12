//
// Created by Niemand on 4/12/16.
//


#include <Settings.h>
#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>

bool Settings::parseCommandLine(int nargs, const char **args) {
  bool success = true;
  try {

    TCLAP::CmdLine cmd("Factorio production drawer", ' ', "alpha");

    TCLAP::SwitchArg dotOutputArg("d", "dot", "Switch on the dot graph formating instead of the ascii formatting.", cmd, false);
    TCLAP::ValueArg<double> speedArg("s", "speed", "The construction speed multiplier for the fabrication. [Default: 1.0]  E.g.: Assembling Machine 1 = 0.5, Assembling Machine 2 = 0.75, Assembling Machine 3 = 1.25", false, 1.0, "speed factor", cmd);
    TCLAP::MultiArg<double> unitsArg("u", "units", "The number of units that the recipe should produce per minute. For each recipe a separate amount can be specified. The last unit is used for the remaining recipes. [Default: 1.0]", false, "Units per minute", cmd);
    TCLAP::UnlabeledMultiArg<std::string> recipesArg("recipe", "The recipes for which the ingredients are displayed. The name can either be the internal factorio name or the natural language description.", true, "recipe", cmd);

    cmd.parse( nargs, args);

    dotOutput = dotOutputArg.isSet();
    recipes = recipesArg.getValue();
    units = unitsArg.getValue();
    if(0 == units.size()) {
      units.push_back(1.0);
    }
    speed = speedArg.getValue();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for argument " << e.argId() << std::endl;
    success = false;
  }

  return success;
}