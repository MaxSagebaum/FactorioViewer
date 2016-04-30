//
// Created by Niemand on 4/10/16.
//

#ifndef PRODUCTION_NODE_H
#define PRODUCTION_NODE_H

#include <vector>
#include <string>

struct ItemData {
  std::string name;
  double units;
  double fabs;
  std::string picture;

  ItemData():
    name(""),
    units(0),
    fabs(0),
    picture("")
  {}

  ItemData(const std::string& name, double units, double fabs):
    name(name),
    units(units),
    fabs(fabs),
    picture("")
  {}

  void add(const ItemData& other) {
    this->units += other.units;
    this->fabs += other.fabs;
  }
};

struct ProductionNode {
  std::vector<ItemData> items;
  std::vector<ProductionNode> children;


  ProductionNode();

  void addItem(const ItemData& item);

  void addChild(const ProductionNode& node);

  void printTree();

  static void printList(const std::vector<ProductionNode>& nodes, size_t maxSize = 5);

  static void outputLines(const std::string *lines, size_t n);

  static void clearLines(std::string *lines, const size_t n);
};

struct RenderData {
  std::vector<std::string> lines;
  size_t items;
  size_t maxSize;

  size_t itemsPlotted;
  size_t childPosition;
  size_t childrenSize;


  RenderData(const size_t items, const size_t childrenSize);

  void addLine(const std::string &line);
};


#endif //FACTORIOVIEWER_ASCIITREE_H
