//
// Created by Niemand on 4/11/16.
//

#include <iostream>
#include <util.hpp>
#include "DotFormatter.h"

void DotFormatter::addTreeNode(const ASCIINode &node, const std::string& designation) {
  std::cout << designation << "[label=\"";
  for (auto &&line : node.lines) {
    std::cout << line << "\\n";
  }
  std::cout << "\"];\n";

  for(int i = 0; i < node.children.size(); ++i) {

    const std::string &childDes = format("%s_%02d", designation.c_str(), i);
    addTreeNode(node.children[i], childDes);

    std::cout << designation << "->" << childDes << ";\n";
  }
}

void DotFormatter::formatTree(const ASCIINode &tree, const std::vector<ASCIINode> &totals) {
  std::cout << "digraph buildTree {" << std::endl;

  addTreeNode(tree, "r");

  for(int i = 0; i < totals.size(); ++i) {
    const std::string &totalDes = format("t_%02d", i);
    addTreeNode(totals[i], totalDes);

    addTotalLinks(tree, totals[i], "r", totalDes);
  }

  std::cout << "}" << std::endl;
}

void DotFormatter::addTotalLinks(const ASCIINode &node, const ASCIINode &totalNode, const std::string &designator,
                                 const std::string &totalDesignator) {

  if(0 == node.lines[0].compare(totalNode.lines[0])) {
    std::cout << designator << " -> " << totalDesignator << ";\n";
  }

  for(int i = 0; i < node.children.size(); ++i) {

    const std::string &childDes = format("%s_%02d", designator.c_str(), i);
    addTotalLinks(node.children[i], totalNode, childDes, totalDesignator);
  }
}

