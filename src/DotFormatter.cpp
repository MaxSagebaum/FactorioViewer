//
// Created by Niemand on 4/11/16.
//

#include <iostream>
#include <util.hpp>
#include <Settings.h>
#include <algorithm>
#include "DotFormatter.h"

const std::string COLOR_REGULAR="black";
const std::string COLOR_TOTAL="blue";

void DotFormatter::printItemNode(const std::vector<ItemData> &items, const std::string& designation, const std::string& color) {
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
  for(size_t i = 0; i < items.size(); ++i) {
    if(0 != i) {
      std::cout << " | ";
    }
    std::cout << items[i].name << "| { Picture | {" << items[i].units << " | " << items[i].fabs << "}}";
  }
  std::cout << "}\"];\n";
}

void DotFormatter::addTreeNode(const ProductionNode &node, const std::string& designation, const std::string& color) {
  printItemNode(node.items, designation, color);

  for(size_t i = 0; i < node.children.size(); ++i) {

    const std::string &childDes = format("%s_%02d", designation.c_str(), i);
    addTreeNode(node.children[i], childDes, color);

    std::cout << designation << "->" << childDes << ";\n";
  }
}

std::string formatIdentifier(const std::string& str) {
  std::string designator = str;
  std::replace(designator.begin(), designator.end(), '-', '_');

  return designator;

}

void DotFormatter::buildSums(const ProductionNode& node, std::map<std::string, ItemData>& sums, std::map<std::string, std::set<std::string> >& links) {
  for(int parentItemPos = 0; parentItemPos < node.items.size(); ++parentItemPos) {
    ItemData& sumNode = sums[node.items[parentItemPos].name];

    if(sumNode.name.length() == 0) {
      // first time node is accessed
      sumNode = node.items[parentItemPos];
    } else {
      sumNode.add(node.items[parentItemPos]);
    }

    for(int childPos = 0; childPos < node.children.size(); ++childPos) {
      const ProductionNode& child = node.children[childPos];
      for(int childItemPos = 0; childItemPos < child.items.size(); ++childItemPos) {
        links[sumNode.name].insert(child.items[childItemPos].name);
      }
    }
  }
  for(int i = 0; i < node.children.size(); ++i) {
    buildSums(node.children[i], sums, links);
  }
}

void DotFormatter::formatTree(const std::vector<ProductionNode> &roots, const std::vector<ProductionNode> &totals, const Settings& settings) {
  std::cout << "digraph buildTree {" << std::endl;
  if(settings.unifyDot) {
    std::map<std::string, ItemData> sums;
    std::map<std::string, std::set<std::string> > links;

    for(int i = 0; i < roots.size(); ++i) {
      buildSums(roots[i], sums, links);
    }

    for(auto&& item : sums) {
      std::vector<ItemData> temp;
      temp.push_back(item.second);
      printItemNode(temp, formatIdentifier(item.first), COLOR_REGULAR);
    }

    for(auto&& link : links) {
      for(auto&& target : link.second) {
        std::cout << formatIdentifier(link.first) << " -> " << formatIdentifier(target) << ";\n";
      }
    }
  } else {

    for(size_t i = 0; i < roots.size(); ++i) {
      addTreeNode(roots[i], format("r_%02d", i), COLOR_REGULAR);
    }

    for(size_t i = 0; i < totals.size(); ++i) {
      const std::string& totalDes = format("t_%02d", i);
      addTreeNode(totals[i], totalDes, COLOR_TOTAL);

      for(size_t j = 0; j < roots.size(); ++j) {
        addTotalLinks(roots[j], totals[i], format("r_%02d", j), totalDes);
      }
    }

    std::cout << "{rank=same;";
    for(size_t i = 0; i < totals.size(); ++i) {
      const std::string& totalDes = format("t_%02d", i);

      std::cout << " " << totalDes;
    }
    std::cout << "}\n";
  }
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

