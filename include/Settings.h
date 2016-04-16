//
// Created by Niemand on 4/12/16.
//

#ifndef FACTORIOVIEWER_SETTINGS_H
#define FACTORIOVIEWER_SETTINGS_H

#include <string>
#include <vector>

struct Settings {

  const std::string INI_NAME = "factorioViewer.ini";

  std::vector<std::string> baseComponents;
  std::vector<std::string> readDirectories;
  std::vector<std::string> recipes;
  std::vector<double> units;
  double speed;
  bool dotOutput;

  bool parseCommandLine(int nargs, const char** args);

  void readIni();

  void writeIni();

};

#endif //FACTORIOVIEWER_SETTINGS_H
