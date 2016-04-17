//
// Created by Niemand on 4/11/16.
//

#include <iostream>
#include <util.hpp>
#include "DotFormatter.h"

const std::string COLOR_REGULAR="black";
const std::string COLOR_TOTAL="blue";

void DotFormatter::addTreeNode(const ProductionNode &node, const std::string& designation, const std::string& color) {
  // output format:
  // +----------------------------+
  // | description                |
  // +----------------------------+
  // |         | units per minute |
  // | Picture +------------------+
  // |         | fabs required    |
  // +----------------------------+
  std::cout << designation << "[shape=record, color=" << color << ", label=\"";
  std::cout << "{";
  for(size_t i = 0; i < node.items.size(); ++i) {
    if(0 != i) {
      std::cout << " | ";
    }
    std::cout << node.items[i].name << "| { Picture | {" << node.items[i].units << " | " << node.items[i].fabs << "}}";
  }
  std::cout << "}\"];\n";

  for(size_t i = 0; i < node.children.size(); ++i) {

    const std::string &childDes = format("%s_%02d", designation.c_str(), i);
    addTreeNode(node.children[i], childDes, color);

    std::cout << designation << "->" << childDes << ";\n";
  }
}

void DotFormatter::formatTree(const std::vector<ProductionNode> &roots, const std::vector<ProductionNode> &totals) {
  std::cout << "digraph buildTree {" << std::endl;

  for(size_t i = 0; i < roots.size(); ++i) {
    addTreeNode(roots[i], format("r_%02d", i), COLOR_REGULAR);
  }

  for(size_t i = 0; i < totals.size(); ++i) {
    const std::string &totalDes = format("t_%02d", i);
    addTreeNode(totals[i], totalDes, COLOR_TOTAL);

    for(size_t j = 0; j < roots.size(); ++j) {
      addTotalLinks(roots[j], totals[i], format("r_%02d", j), totalDes);
    }
  }

  std::cout << "{rank=same;";
  for(size_t i = 0; i < totals.size(); ++i) {
    const std::string &totalDes = format("t_%02d", i);

    std::cout << " " << totalDes;
  }
  std::cout << "}\n";

  std::cout << "}" << std::endl;
}

void DotFormatter::addTotalLinks(const ProductionNode &node, const ProductionNode &totalNode, const std::string &designator,
                                 const std::string &totalDesignator) {

  for(size_t i = 0; i < node.items.size(); ++i) {
    if(0 == node.items[i].name.compare(totalNode.items[0].name)) {
      std::cout << designator << " -> " << totalDesignator << ";\n";
    }
  }

  for(size_t i = 0; i < node.children.size(); ++i) {

    const std::string &childDes = format("%s_%02d", designator.c_str(), i);
    addTotalLinks(node.children[i], totalNode, childDes, totalDesignator);
  }
}

