//
// Created by Niemand on 4/11/16.
//

#ifndef FACTORIOVIEWER_DOTFORMATTER_H
#define FACTORIOVIEWER_DOTFORMATTER_H


#include "ASCIITree.h"

class DotFormatter {

  void addTreeNode(const ProductionNode &node, const std::string &designation, const std::string& color);

  void addTotalLinks(const ProductionNode &tree, const ProductionNode &totalNode, const std::string &designator,
                     const std::string &totalDesignator);

public:
  void formatTree(const std::vector<ProductionNode>& roots, const std::vector<ProductionNode>& totals);
};


#endif //FACTORIOVIEWER_DOTFORMATTER_H
