//
// Created by Niemand on 4/11/16.
//

#ifndef FACTORIOVIEWER_DOTFORMATTER_H
#define FACTORIOVIEWER_DOTFORMATTER_H


#include "ProductionNode.h"
#include "Settings.h"

#include <map>
#include <set>

class DotFormatter {

  void printItemNode(const std::vector<ItemData> &items, const std::string& designation, const std::string& color);
  void addTreeNode(const ProductionNode &node, const std::string &designation, const std::string& color);

  void addTotalLinks(const ProductionNode &tree, const ProductionNode &totalNode, const std::string &designator,
                     const std::string &totalDesignator);

  void buildSums(const ProductionNode& node, std::map<std::string, ItemData>& sums, std::map<std::string, std::set<std::string> >& links);

public:
  void formatTree(const std::vector<ProductionNode> &roots, const std::vector<ProductionNode> &totals, const Settings& settings);
};


#endif //FACTORIOVIEWER_DOTFORMATTER_H
