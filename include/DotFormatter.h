//
// Created by Niemand on 4/11/16.
//

#ifndef FACTORIOVIEWER_DOTFORMATTER_H
#define FACTORIOVIEWER_DOTFORMATTER_H


#include "ASCIITree.h"

class DotFormatter {

  void addTreeNode(const ASCIINode &node, const std::string &designation);

  void addTotalLinks(const ASCIINode &tree, const ASCIINode &totalNode, const std::string &designator,
                     const std::string &totalDesignator);

public:
  void formatTree(const std::vector<ASCIINode>& roots, const std::vector<ASCIINode>& totals);
};


#endif //FACTORIOVIEWER_DOTFORMATTER_H
