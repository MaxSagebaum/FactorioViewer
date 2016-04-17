//
// Created by Niemand on 4/11/16.
//

#include <iostream>
#include <util.hpp>
#include "DotFormatter.h"

const std::string COLOR_REGULAR="black";
const std::string COLOR_TOTAL="blue";

void DotFormatter::addTreeNode(const ASCIINode &node, const std::string& designation, const std::string& color) {
  // output format:
  // +----------------------------+
  // | description                |
  // +----------------------------+
  // |         | units per minute |
  // | Picture +------------------+
  // |         | fabs required    |
  // +----------------------------+
  std::cout << designation << "[shape=record, color=" << color << ", label=\"";
  std::cout << "{" << node.lines[0] << "| { Picture | {" << node.lines[1] << " | " << node.lines[2] << "}}}";
  std::cout << "\"];\n";

  for(int i = 0; i < node.children.size(); ++i) {

    const std::string &childDes = format("%s_%02d", designation.c_str(), i);
    addTreeNode(node.children[i], childDes, color);

    std::cout << designation << "->" << childDes << ";\n";
  }
}

void DotFormatter::formatTree(const std::vector<ASCIINode> &roots, const std::vector<ASCIINode> &totals) {
  std::cout << "digraph buildTree {" << std::endl;

  for(int i = 0; i < roots.size(); ++i) {
    addTreeNode(roots[i], format("r_%02d", i), COLOR_REGULAR);
  }

  for(int i = 0; i < totals.size(); ++i) {
    const std::string &totalDes = format("t_%02d", i);
    addTreeNode(totals[i], totalDes, COLOR_TOTAL);

    for(int j = 0; j < roots.size(); ++j) {
      addTotalLinks(roots[j], totals[i], format("r_%02d", j), totalDes);
    }
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

