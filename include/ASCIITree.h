//
// Created by Niemand on 4/10/16.
//

#ifndef FACTORIOVIEWER_ASCIITREE_H
#define FACTORIOVIEWER_ASCIITREE_H

#include <vector>
#include <string>

struct ItemData {
  std::string name;
  double units;
  double fabs;
  std::string picture;

  ItemData(const std::string& name, double units, double fabs):
    name(name),
    units(units),
    fabs(fabs),
    picture("")
  {}
};

struct ASCIINode {
  std::vector<ItemData> items;
  std::vector<ASCIINode> children;


  ASCIINode();

  void addItem(const ItemData& item);

  void addChild(const ASCIINode& node);

  void printTree();

  static void printList(const std::vector<ASCIINode>& nodes, size_t maxSize = 5);

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
