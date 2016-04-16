//
// Created by Niemand on 4/12/16.
//

#ifndef FACTORIOVIEWER_SETTINGS_H
#define FACTORIOVIEWER_SETTINGS_H

#include <string>
#include <vector>

static const char* const INI_DIRECTORIES = "directories";

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

  void splitList(const std::string& line, std::vector<std::string>& items);

  template<typename Type>
  void handleEntry(const std::string& targetName, const std::string& name, const std::string& value, Type& targetValue);
  template<typename Type>
  void handleEntryList(const std::string& targetName, const std::string& name, const std::string& value,
                       std::vector<Type>& targetValues);

  template<typename Type>
  void outputEntryList(std::ofstream& out, const std::string& name, const std::vector<Type>& values);
};
#endif //FACTORIOVIEWER_SETTINGS_H
