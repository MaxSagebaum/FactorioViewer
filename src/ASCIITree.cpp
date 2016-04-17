//
// Created by Niemand on 4/10/16.
//

#include <iostream>
#include <util.hpp>
#include "../include/ASCIITree.h"

const std::string empty = "       ";
const std::string bar = "   |   ";
const std::string arrowRight = " ----> ";
const std::string arrowRightWithChild = " --+-> ";
const std::string arrowRightEnd = "   +-> ";

const std::string arrowLeft = " <---- ";
const std::string arrowLeftWithChild = " <-+-- ";
const std::string arrowLefttEnd = " <-+-- ";

const std::string line = " ----- ";
const std::string lineWithChild = " --+-- ";


RenderData::RenderData(const size_t items, const size_t childrenSize) :
  lines(),
  items(items),
  maxSize(0),

  itemsPlotted(0),
  childPosition(0),
  childrenSize(childrenSize)
{ }

void RenderData::addLine(const std::string& line) {
  lines.push_back(line);
  if(maxSize < line.size()) {
    maxSize = line.size();
  }
}

ProductionNode::ProductionNode() :
  items(),
  children()
{ }

void ProductionNode::addChild(const ProductionNode &node) {
  this->children.push_back(node);
}

void ProductionNode::addItem(const ItemData &item) {
  this->items.push_back(item);
}

std::string addToLine(const std::string& text, size_t maxSize) {
  std::string result;
  result += text;
  for(size_t i = text.size(); i < maxSize; ++i) {
    result += " ";
  }

  return result;
}

void addToLines(const size_t n, std::string* lines, std::string& filler, const std::string* items, const size_t maxSize) {
  for(size_t i = 0; i < n; ++i) {
    lines[i] += addToLine(items[i], maxSize);
  }
  filler += addToLine("", maxSize);
}

void addToLines(const size_t n, std::string* lines, std::string& filler, const std::string& item, const size_t maxSize) {
  for(size_t i = 0; i < n; ++i) {
    lines[i] += addToLine(item, maxSize);
  }
  filler += addToLine("", maxSize);
}

void addTree(const size_t n, std::string* lines, std::string& filler, const size_t pos, const std::string& append) {
  if(pos < n) {
    for(size_t i = pos; i < n; ++i) {
      lines[i] += append;
    }
  }

  filler += append;
}

template<typename ... Args>
void addTree(const size_t n, std::string* lines, std::string& filler, const size_t pos, const std::string& append,  const Args& ... args) {
  if(pos < n) {
    lines[pos] += append;
    addTree(n, lines, filler, pos + 1, args...);
  } else if(pos == n) {
    filler += append;
  }
}

void printLine(std::vector<RenderData>& nodeStack, size_t linesPerItem) {
  std::string* lines = new std::string[linesPerItem];
  std::string filler;
  bool outputFiller = false;
  for(size_t i = 0; i < nodeStack.size(); ++i) {
    RenderData& data = nodeStack[i];

    if(data.itemsPlotted < data.items) {

      addToLines(linesPerItem, lines, filler, &data.lines[data.itemsPlotted * linesPerItem], data.maxSize);

      if(data.childrenSize == 1) {
        addTree(linesPerItem, lines, filler, 0, empty, arrowRight, empty);
      } else if(data.childrenSize > 1) {
        addTree(linesPerItem, lines, filler, 0, empty, arrowRightWithChild, bar);
        outputFiller = true;
      }

      data.itemsPlotted += 1;
    } else {
      addToLines(linesPerItem, lines, filler, "", data.maxSize);

      if(data.childrenSize > 0) {

        if(nodeStack[i + 1].itemsPlotted >= nodeStack[i + 1].items) { // currently not plotting one of the own children
          if(data.childPosition + 1 < data.childrenSize) {
            addTree(linesPerItem, lines, filler, 0, bar);
            outputFiller = true;
          } else {
            addTree(linesPerItem, lines, filler, 0, empty);
          }
        } else {
          if(data.childPosition + 1 == data.childrenSize) {
            addTree(linesPerItem, lines, filler, 0, bar, arrowRightEnd, empty);
          } else {
            addTree(linesPerItem, lines, filler, 0, bar, arrowRightEnd, bar);
            outputFiller = true;
          }
        }
      }
    }
  }

  ProductionNode::outputLines(lines, 3);
  if(outputFiller) {
    std::cout << filler << std::endl;
  }

  delete [] lines;
}

RenderData renderNode(const ProductionNode& node) {
  RenderData data(node.items.size(), node.children.size());
  for(size_t i = 0; i < node.items.size(); ++i) {
    const ItemData& item = node.items[i];

    data.addLine(item.name);
    data.addLine(format("%8.3f u/min", item.units));
    data.addLine(format("%8.3f fabs", item.fabs));
  }

  return data;
}

void addAndPrint(ProductionNode& node, std::vector<RenderData>& nodeStack) {
  if(node.children.size() == 0) {
    printLine(nodeStack, 3);
  } else {
    for (size_t i = 0; i < node.children.size(); ++i) {
      nodeStack.push_back(renderNode(node.children[i]));
      addAndPrint(node.children[i], nodeStack);

      nodeStack.pop_back();

      nodeStack.back().childPosition += 1;
    }
  }
}

void ProductionNode::printTree() {
  std::vector<RenderData> nodeStack;

  nodeStack.push_back(renderNode(*this));
  addAndPrint(*this, nodeStack);
}

void ProductionNode::printList(const std::vector<ProductionNode> &nodes, size_t maxSize) {

  const size_t n = 3;
  std::string lines[n];
  std::string filler;

  bool firstOut = true;
  size_t pos = 0;
  for (auto &&node : nodes) {
    RenderData data = renderNode(node);
    if(0 != pos) {
      addTree(n, lines, filler, 0, empty);
    }

    addToLines(n, lines, filler, &data.lines[0], data.maxSize);

    pos += 1;
    if(pos >= maxSize) {
      if(!firstOut) {
        std::cout << std::endl;
      } else {
        firstOut = false;
      }
      outputLines(lines, n);
      clearLines(lines, n);
      pos = 0;
    }
  }

  if(pos > 0) {
    if(!firstOut) {
      std::cout << std::endl;
    }
    outputLines(lines, n);
  }
}

void ProductionNode::clearLines(std::string *lines, const size_t n) {
  for(size_t i = 0; i < n; ++i) {
        lines[i].clear();
      }
}

void ProductionNode::outputLines(const std::string *lines, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::cout << lines[i] << std::endl;
  }
}



