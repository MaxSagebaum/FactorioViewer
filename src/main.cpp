#include <iostream>
#include <sstream>

#include<lua.hpp>
#include <vector>
#include <map>
#include <ProductionNode.h>
#include <util.hpp>
#include <DotFormatter.h>
#include <Settings.h>
#include <dirent.h>

using namespace std;

struct Part {
  std::string name;
  double quantity;
  std::string type;

  Part() :
      name(),
      quantity(0),
      type("item") {}
};

struct Recipe {
  std::string name;
  std::string creates;
  std::vector<Part> parts;
  std::vector<Part> results;
  double time;
  double yield;

  Recipe() :
      name(),
      creates(),
      parts(),
      time(0.5),
      yield(1) {}

  void finish() {
    if(0 == this->results.size()) {
      Part part;
      part.name = this->creates;
      part.quantity = this->yield;
      this->results.push_back(part);
    }
  }
};

void report_errors(lua_State *L, int status) {
  if (status != 0) {
    std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message

    exit(1);
  }
}

std::string getString(lua_State *L, int stackItem) {
  std::stringstream stream;
  lua_pushvalue(L, stackItem);

  if (lua_isstring(L, -1)) {
    stream << lua_tostring(L, -1);
  } else if (lua_isnumber(L, -1)) {
    stream << lua_tonumber(L, -1);
  } else if(lua_isboolean(L, -1)) {
    stream << lua_toboolean(L, -1);
  } else if(lua_isnoneornil(L, -1)) {
    // ignore
  } else {
    //printf("Error unknown type: %s\n", lua_typename(L, -1));
  }
  lua_pop(L, 1);

  return stream.str();
}

int getInteger(lua_State *L, int stackItem) {
  int value;
  lua_pushvalue(L, stackItem);

  if (lua_isnumber(L, -1)) {
    value = (int)lua_tonumber(L, -1);
  } else if(lua_isinteger(L, -1)) {
    value = lua_tointeger(L, -1);
  } else {
    std::cerr << "Expecting double or integer" << std::endl;
    std::cerr << lua_typename(L, lua_type(L, -1)) << std::endl;
    exit(-1);
  }

  lua_pop(L, 1);

  return value;
}

void PrintTable(lua_State *L, int deep);

void readPart(Part& part, lua_State *L) {
  if (0 == lua_istable(L, -1)) {
    std::cerr << "Expecting table as value" << std::endl;
    exit(-1);
  }
  //PrintTable(L, 5);
  lua_pushnil(L);

  int pos = 0;
  while (lua_next(L, -2) != 0) {
    if (1 == lua_isinteger(L, -2)) {
      if (0 == pos) {
        part.name = getString(L, -1);
      } else if ( 1 == pos) {
        part.quantity = (double) getInteger(L, -1);
      } else {
        std::cerr << "Position to large: " << pos << std::endl;
        exit(-1);
      }
      ++pos;
    } else if(1 == lua_isstring(L, -2)) {
      std::string key = getString(L, -2);
      std::string value = getString(L, -1);

      if (0 == key.compare("name")) {
        part.name = getString(L, -1);
      } else if (0 == key.compare("amount")) {
        part.quantity = (double) getInteger(L, -1);
      } else if (0 == key.compare("type")) {
        part.type = getString(L, -1);
      } else {
        std::cerr << "Unknown key: " << key << " " << value << std::endl;
        exit(-1);
      }
    } else {
      std::cerr << "Expecting string or integer as key" << std::endl;
      std::cerr << lua_typename(L, lua_type(L, -2)) << std::endl;
      exit(-1);
    }
    lua_pop(L, 1);
  }
}

void readParts(std::vector<Part>& parts, lua_State *L) {
  if (0 == lua_istable(L, -1)) {
    std::cerr << "Expecting table as value" << std::endl;
    exit(-1);
  }
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    if (0 == lua_isinteger(L, -2)) {
      std::cerr << "Expecting integer as key" << std::endl;
      exit(-1);
    }

    Part part;
    readPart(part, L);
    parts.push_back(part);

    lua_pop(L, 1);
  }
}

void readRecipe(Recipe& recipe, lua_State *L) {
  if (0 == lua_istable(L, -1)) {
    std::cerr << "Expecting table as value" << std::endl;
    exit(-1);
  }

  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    if (0 == lua_isstring(L, -2)) {
      std::cerr << "Expecting string as key" << std::endl;
      exit(-1);
    }
    std::string key = getString(L, -2);

    if(0 == key.compare("enabled") || 0 == key.compare("type") || 0 == key.compare("category") || 0 == key.compare("hidden") || 0 == key.compare("order") || 0 == key.compare("icon")|| 0 == key.compare("subgroup")) {
      // ignore
    } else if(0 == key.compare("name")) {
      recipe.name = getString(L, -1);
    } else if(0 == key.compare("result") || 0 == key.compare("main_product")) {
      recipe.creates = getString(L, -1);
    } else if(0 == key.compare("energy_required")) {
      recipe.time = (double)getInteger(L, -1);
    } else if(0 == key.compare("result_count")) {
      recipe.yield = (double)getInteger(L, -1);
    } else if(0 == key.compare("ingredients")) {
      readParts(recipe.parts, L);
    } else if(0 == key.compare("results")) {
      readParts(recipe.results, L);
    } else {
      std::cerr << "Unknown key: " <<  key << std::endl;
      exit(-1);
    }

    lua_pop(L, 1);
  }

  recipe.finish();
}

void readRecipes(std::map<std::string, Recipe>& recipes, lua_State *L) {
  if (0 == lua_istable(L, -1)) {
    std::cerr << "Expecting table as value" << std::endl;
    exit(-1);
  }

  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    if (0 == lua_isinteger(L, -2)) {
      break;
    }
    if (0 == lua_istable(L, -1)) {
      std::cerr << "Expecting table as value" << std::endl;
      exit(-1);
    }

    Recipe recipe;
    readRecipe(recipe, L);
    recipes[recipe.name] = recipe;

    lua_pop(L, 1);
  }
}

// In Lua 5.0 reference manual is a table traversal example at page 29.
void PrintTable(lua_State *L, int deep) {
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    if(deep < 3) {
      if (lua_isstring(L, -2) || lua_isinteger(L, -2)) {
        printf("%s = ", getString(L, -2).c_str());
      } else {
        printf("Error unknown key type: %s\n", lua_typename(L, -2));
      }

      if (lua_istable(L, -1)) {
        printf("{\n");
        PrintTable(L, deep + 1);
        printf("}\n");
      } else {
        printf("%s\n", getString(L, -1).c_str());
      }
    } else {
      printf("%s - %s\n",
             lua_typename(L, lua_type(L, -2)),
             lua_typename(L, lua_type(L, -1)));

    }

    lua_pop(L, 1);
  }
}

bool isBaseIngredient(const std::string& name, const Settings& settings) {
  for(auto base : settings.baseComponents) {
    if(0 == base.compare(name)) {
      return true;
    }
  }
  return false;
}

bool isTotalIngredient(const std::string& name, const Settings& settings) {
  for(auto total : settings.totalComponents) {
    if(0 == total.compare(name)) {
      return true;
    }
  }
  return false;
}

void addItem(const Recipe &recipe, double amount, ProductionNode &node, std::map<std::string,double>& totals, const Settings& settings) {

  for(size_t i = 0; i < recipe.results.size(); ++i) {
    double fabs = recipe.time * amount / (60.0 * recipe.results[i].quantity * settings.speed);
    double units = recipe.results[i].quantity * amount;
    node.addItem(ItemData(recipe.results[i].name, units, fabs));

    if(settings.totalAll ||
       isTotalIngredient(recipe.results[i].name, settings)) {
      totals[recipe.results[i].name] += amount;
    }
  }
}

ProductionNode outputYield(const std::map<std::string, const Recipe*>& targetToRecipe, const Recipe& recipe, double amount, std::map<std::string,double>& totals, const Settings& settings) {
  ProductionNode node;

  addItem(recipe, amount, node, totals, settings);

  for(const Part& part : recipe.parts) {

    if(targetToRecipe.find(part.name) != targetToRecipe.end()) {
      if(!isBaseIngredient(part.name, settings)) {
        node.addChild(
            outputYield(targetToRecipe, *targetToRecipe.at(part.name), amount * part.quantity, totals, settings));
      } else {
        ProductionNode child;
        addItem(*targetToRecipe.at(part.name), amount * part.quantity, child, totals, settings);
        node.addChild(child);
      }
    } else {
      std::cerr << "Could not find recipe for : " << part.name << std::endl;
    }
  }

  return node;
}

std::vector<ProductionNode> outputTotals(std::map<std::string, Recipe>& recipes, const std::map<std::string, double>& totals, const Settings& settings) {
  std::vector<ProductionNode> vector(totals.size());
  std::map<std::string,double> temp;

  int pos = 0;
  for ( auto iter = totals.begin(); iter != totals.end(); iter++) {
    addItem(recipes[iter->first], iter->second, vector[pos], temp, settings);
    pos += 1;
  }

  return vector;
}

void listLuaFiles(const std::string& directory, std::vector<std::string>& files) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (directory.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      std::string fileName = ent->d_name;

      if(std::string::npos != fileName.rfind(".lua")) {
        files.push_back(directory + "/" + fileName);
      }
    }
    closedir (dir);
  } else {
    std::cerr << "Warning: Could not open the directory: '" << directory << "'." << std::endl;
  }
}

void buildTargetToRecipeLookup(const std::map<std::string, Recipe>& recipes, std::map<std::string, const Recipe*>& targetToRecipe) {
  for(auto&& recipeEntry : recipes) {
    const Recipe& recipe = recipeEntry.second;

    for(size_t i = 0; i < recipe.results.size(); ++i) {
      targetToRecipe[recipe.results[i].name] = &recipe;
    }
  }
}

int main(int nargs, const char **args) {

  Settings settings;
  if(settings.parseCommandLine(nargs, args)) {
    lua_State *L = luaL_newstate();

    lua_createtable(L, 0, 0);
    lua_setglobal(L,"data");

    std::vector<std::string> files;
    for(auto dir : settings.readDirectories) {
      listLuaFiles(dir, files);
    }

    // Load file.
    report_errors(L, luaL_loadfile(L, "lua/extend.lua"));
    report_errors(L, lua_pcall(L, 0, 0, 0));
    for(auto file : files) {
      report_errors(L, luaL_loadfile(L, file.c_str()));
      report_errors(L, lua_pcall(L, 0, 0, 0));
    }

    std::map<std::string, Recipe> recipes;

    // Print table contents.
    lua_getglobal(L, "data");
    readRecipes(recipes, L);
    lua_close(L);


    // base setup finished
    if(settings.list) {
      // print all recipes
      for(auto&& item : recipes) {
        std::cout << item.second.name << std::endl;
      }
    } else {
      // print the output for all give recipes

      std::map<std::string, const Recipe *> targetToRecipe;
      buildTargetToRecipeLookup(recipes, targetToRecipe);

      //std::cout << "Recipes: " << recipes.size() << std::endl;

      std::map<std::string, double> totals;

      std::vector<ProductionNode> nodes;
      for (size_t i = 0; i < settings.recipes.size(); ++i) {
        double units = settings.units.back();
        if (i < settings.units.size()) {
          units = settings.units[i];
        }
        if (recipes.find(settings.recipes[i]) != recipes.end()) {
          ProductionNode node = outputYield(targetToRecipe, recipes[settings.recipes[i]], units, totals, settings);
          nodes.push_back(node);
        } else {
          std::cerr << "Could not find a recipe for: " << settings.recipes[i] << std::endl;
        }
      }
      std::vector<ProductionNode> totalNodes = outputTotals(recipes, totals, settings);


      if (settings.dotOutput) {
        DotFormatter dotFormatter;
        dotFormatter.formatTree(nodes, totalNodes);
      } else {
        for (size_t i = 0; i < nodes.size(); ++i) {
          nodes[i].printTree();
          std::cout << std::endl;
        }
        ProductionNode::printList(totalNodes, 5);
      }
    }

    return 0;
  } else {
    return -1;
  }
}
