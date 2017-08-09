//
// Created by Niemand on 4/10/16.
//

#include <iostream>
#include <util.hpp>
#include "ProductionNode.h"

const std::string empty = "       ";
const std::string bar = "   |   ";
const std::string arrowRight = " ----> ";
const std::string arrowRightWithChild = " --+-> ";
const std::string arrowRightEnd = "   +-> ";

const std::string arrowLeft = " <-+   ";

const std::string line = " ------";
const std::string lineWithChild = " --+---";


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

size_t RenderData::getLinesPerItem() {
  return lines.size() / items;
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

void printLine(std::vector<RenderData>& nodeStack) {
  const size_t linesPerItem = nodeStack[0].getLinesPerItem();
  std::string* lines = new std::string[linesPerItem];
  std::string filler;
  bool outputFiller = false;
  for(size_t i = 0; i < nodeStack.size(); ++i) {
    RenderData& data = nodeStack[i];

    bool isFirst = false;
    if(data.itemsPlotted < data.items) {
      if(data.itemsPlotted == 0) {
        isFirst = true;
      }

      addToLines(linesPerItem, lines, filler, &data.lines[data.itemsPlotted * linesPerItem], data.maxSize);

      if(data.items > 1) { // add extra arrows to the left and spacing
        const std::string* fromTop = &bar;     // Default:  |
        const std::string* arrow = &arrowLeft; //          <+
        const std::string* below = &empty;       //
        if(data.itemsPlotted == 0) {   // first has no bar from top and only a line with child
          fromTop = &empty;
          arrow = &lineWithChild;
        }
        if(data.itemsPlotted + 1 != data.items) { // last hast no bar below
          below = &bar;
          outputFiller = true;
        }

        addTree(linesPerItem, lines, filler, 0, *fromTop, *arrow, *below);
      }

      data.itemsPlotted += 1;
    } else {
      addToLines(linesPerItem, lines, filler, "", data.maxSize);
      if(data.items > 1) {
        addTree(linesPerItem, lines, filler, 0, empty);  // all results are plotted so only empty space needed
      }
    }

    if(data.childPosition < data.childrenSize) {
      // a straight line is the default
      const std::string* fromTop = &bar;
      const std::string* arrow = &bar;
      const std::string* below = &bar;

      if(isFirst) {
        fromTop = &empty;
        if(data.childrenSize == 1) {
          arrow = &arrowRight;
          below = &empty;
        } else {
          arrow = &arrowRightWithChild;
          outputFiller = true;
        }
      } else {
        if(0 == nodeStack[i + 1].itemsPlotted) { // one of my children starts plotting
          arrow = &arrowRightEnd;
          if(data.childPosition + 1 == data.childrenSize) {
             below = &empty;
          } else {
            outputFiller = true;
          }
        } else {
          if(data.childPosition + 1 == data.childrenSize) {
            // no children left
            fromTop = &empty;
            arrow = &empty;
            below = &empty;
          } else {
            outputFiller = true;
          }
        }
      }

      addTree(linesPerItem, lines, filler, 0, *fromTop, *arrow, *below);
    }
  }

  ProductionNode::outputLines(lines, linesPerItem);
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
    if(-1.0 == item.fabs) {
      data.addLine("");
    } else {
      data.addLine(format("%8.3f fabs", item.fabs));
    }
    data.addLine(format("%8.3f belts", item.units / (60.0 * 13.33)));
  }

  return data;
}

void addAndPrint(ProductionNode& node, std::vector<RenderData>& nodeStack) {
  if(node.children.size() == 0) {
    printLine(nodeStack);
  } else {
    for (size_t i = 0; i < node.children.size(); ++i) {
      nodeStack.push_back(renderNode(node.children[i]));
      addAndPrint(node.children[i], nodeStack);

      // print the remaining lines
      while(nodeStack.back().itemsPlotted < nodeStack.back().items) {
        printLine(nodeStack);
      }
      nodeStack.pop_back();

      nodeStack.back().childPosition += 1;
    }
  }
}

void ProductionNode::printTree() {
  std::vector<RenderData> nodeStack;

  nodeStack.push_back(renderNode(*this));
  addAndPrint(*this, nodeStack);
  // print the remaining lines
  while(nodeStack.back().itemsPlotted < nodeStack.back().items) {
    printLine(nodeStack);
  }
}

void ProductionNode::printList(const std::vector<ProductionNode> &nodes, size_t maxSize) {

  RenderData tempData = renderNode(nodes[0]);
  const size_t linesPerItem = tempData.getLinesPerItem();
  std::string* lines = new std::string[linesPerItem];
  std::string filler;

  bool firstOut = true;
  size_t pos = 0;
  for (auto &&node : nodes) {
    RenderData data = renderNode(node);

    for(size_t item = 0; item < data.items; ++item) {
      if(0 != pos) {
        addTree(linesPerItem, lines, filler, 0, empty);
      }

      addToLines(linesPerItem, lines, filler, &data.lines[item * linesPerItem], data.maxSize);

      pos += 1;
      if(pos >= maxSize) {
        if(!firstOut) {
          std::cout << std::endl;
        } else {
          firstOut = false;
        }
        outputLines(lines, linesPerItem);
        clearLines(lines, linesPerItem);
        pos = 0;
      }
    }
  }

  if(pos > 0) {
    if(!firstOut) {
      std::cout << std::endl;
    }
    outputLines(lines, linesPerItem);
  }

  delete [] lines;
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



