//
// Created by Niemand on 4/12/16.
//

#ifndef FACTORIOVIEWER_SETTINGS_H
#define FACTORIOVIEWER_SETTINGS_H

#include <string>
#include <vector>

struct Settings {

  std::vector<std::string> recipes;
  std::vector<double> units;
  double speed;
  bool dotOutput;

  bool parseCommandLine(int nargs, const char** args);
};

#endif //FACTORIOVIEWER_SETTINGS_H
