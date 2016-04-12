#include <iostream>
#include <sstream>

#include<lua.hpp>
#include <vector>
#include <map>
#include <ASCIITree.h>
#include <util.hpp>
#include <DotFormatter.h>
#include <Settings.h>

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

bool isBaseIngredient(const std::string& name) {
  return name.compare("iron-ore") == 0 || name.compare("copper-ore") == 0;
}

void formatNode(const Recipe &recipe, double amount, ASCIINode &node, const Settings& settings) {
  double fabs = recipe.time * amount / (60.0 * recipe.yield * settings.speed);

  node.addLine(recipe.creates);
  node.addLine(format("%8.3f u/min", amount));
  node.addLine(format("%8.3f fabs", fabs));
}

ASCIINode outputYield(const std::map<std::string, Recipe>& recipes, const std::string& target, double amount, std::map<std::string,double>& totals, const Settings& settings) {
  ASCIINode node;
  if(recipes.find(target) != recipes.end()) {
    const Recipe& recipe = recipes.at(target);

    totals[target] += amount;
    formatNode(recipe, amount, node, settings);

    for(const Part& part : recipe.parts) {
      if(!isBaseIngredient(part.name)) {
        node.addChild(outputYield(recipes, part.name, amount * part.quantity, totals, settings));
      }
    }
  } else {
    std::cerr << "Could not find recipe: " << target << std::endl;
  }

  return node;
}

std::vector<ASCIINode> outputTotals(std::map<std::string, Recipe>& recipes, const std::map<std::string, double>& totals, const Settings& settings) {
  std::vector<ASCIINode> vector(totals.size());

  int pos = 0;
  for ( auto iter = totals.begin(); iter != totals.end(); iter++) {
    formatNode(recipes[iter->first], iter->second, vector[pos], settings);
    pos += 1;
  }

  return vector;
}

std::vector<std::string> files = {"test/ammo.lua", "test/capsule.lua", "test/demo-furnace-recipe.lua", "test/demo-recipe.lua", "test/demo-turret.lua", "test/equipment.lua", "test/fluid-recipe.lua", "test/furnace-recipe.lua", "test/inserter.lua", "test/module.lua", "test/recipe.lua", "test/turret.lua"};

int main(int nargs, const char **args) {

  Settings settings;
  if(settings.parseCommandLine(nargs, args)) {
    lua_State *L = luaL_newstate();

    lua_createtable(L, 0, 0);
    lua_setglobal(L,"data");

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

    //std::cout << "Recipes: " << recipes.size() << std::endl;

    std::map<std::string, double> totals;

    std::vector<ASCIINode> nodes;
    for(int i = 0; i < settings.recipes.size(); ++i) {
      double units = settings.units.back();
      if(i < settings.units.size()) {
        units = settings.units[i];
      }
      ASCIINode node = outputYield(recipes, settings.recipes[i], units, totals, settings);
      nodes.push_back(node);
    }
    std::vector<ASCIINode> totalNodes = outputTotals(recipes, totals, settings);


    if(settings.dotOutput) {
      DotFormatter dotFormatter;
      dotFormatter.formatTree(nodes, totalNodes);
    } else {
      for(int i = 0; i < nodes.size(); ++i) {
        nodes[i].printTree();
        std::cout << std::endl;
      }
      ASCIINode::printList(totalNodes, 5);
    };

    fflush(stdout);

    lua_close(L);

    return 0;
  } else {
    return -1;
  }
}
