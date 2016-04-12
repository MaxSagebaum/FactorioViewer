//
// Created by Niemand on 4/10/16.
//

#include <iostream>
#include "../include/ASCIITree.h"

const std::string empty = "       ";
const std::string bar = "   |   ";
const std::string arrowStart = " ----> ";
const std::string arrowStartWithChild = " --+-> ";
const std::string arrowEnd = "   +-> ";

ASCIINode::ASCIINode() :
  lines(),
  maxSize(0),
  children(),
  pos(0)
{ }

void ASCIINode::addChild(const ASCIINode &node) {
  this->children.push_back(node);
}

void ASCIINode::addLine(const std::string &line) {
  if(this->maxSize < line.size()) {
    this->maxSize = line.size();
  }

  this->lines.push_back(line);
}

std::string addToLine(const std::string& text, int maxSize) {
  std::string result;
  result += text;
  for(int i = text.size(); i < maxSize; ++i) {
    result += " ";
  }

  return result;
}

void printLine(std::vector<ASCIINode*>& nodeStack, std::vector<int>& childPosition, std::vector<bool>& plotted) {
  std::string lines[3];
  std::string filler;
  bool outputFiller = false;
  for(int i = 0; i < nodeStack.size(); ++i) {
    ASCIINode& node = *nodeStack[i];
    int childPos = childPosition[i];

    if(!plotted[i]) {
      plotted[i] = true;

      lines[0] += addToLine(node.lines[0], node.maxSize);
      lines[1] += addToLine(node.lines[1], node.maxSize);
      lines[2] += addToLine(node.lines[2], node.maxSize);
      filler += addToLine("", node.maxSize);


      if(node.children.size() == 1) {
        lines[0] += empty;
        lines[1] += arrowStart;
        lines[2] += empty;
        filler += empty;
      } else if(node.children.size() > 1) {
        lines[0] += empty;
        lines[1] += arrowStartWithChild;
        lines[2] += bar;
        filler += bar;
        outputFiller = true;
      }
    } else {
      for(int i = 0; i < 3; ++i) {
        lines[i] += addToLine("", node.maxSize);
      }
      filler += addToLine("", node.maxSize);

      if(node.children.size() > 0) {

        if(plotted[i + 1]) {
          if (childPos + 1 < node.children.size()) {
            ASCIINode::appendLines(lines, 3, bar);
            filler += bar;
            outputFiller = true;
          } else {
            ASCIINode::appendLines(lines, 3, empty);
            filler += empty;
          }
        } else {
          lines[0] += bar;
          lines[1] += arrowEnd;

          if(childPos + 1 == node.children.size()) {
            lines[2] += empty;
            filler += empty;
          } else {
            lines[2] += bar;
            filler += bar;
            outputFiller = true;
          }
        }
      }
    }
  }

  ASCIINode::outputLines(lines, 3);
  if(outputFiller) {
    std::cout << filler << std::endl;
  }
}

void addAndPrint(std::vector<ASCIINode*>& nodeStack, std::vector<int>& childPos, std::vector<bool>& plotted) {
  ASCIINode& node = *nodeStack.back();

  if(node.children.size() == 0) {
    printLine(nodeStack, childPos, plotted);
  } else {
    for (int i = 0; i < node.children.size(); ++i) {
      nodeStack.push_back(&node.children[i]);
      childPos.push_back(0);
      plotted.push_back(false);
      addAndPrint(nodeStack, childPos, plotted);

      plotted.pop_back();
      childPos.pop_back();
      nodeStack.pop_back();

      childPos.back() += 1;
    }
  }
}

void ASCIINode::printTree() {
  std::vector<ASCIINode*> nodeStack;
  std::vector<int> childPosition;
  std::vector<bool> plotted;

  nodeStack.push_back(this);
  childPosition.push_back(0);
  plotted.push_back(false);
  addAndPrint(nodeStack, childPosition, plotted);
}

void ASCIINode::printList(const std::vector<ASCIINode> &nodes, int maxSize) {

  const int n = 3;
  std::string lines[n];

  bool firstOut = true;
  int pos = 0;
  for (auto &&node : nodes) {
    if(0 != pos) {
      appendLines(lines, n, empty);
    }

    for(int i = 0; i < n; ++i) {
      lines[i] += addToLine(node.lines[i], node.maxSize);
    }

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

void ASCIINode::clearLines(std::string *lines, const int n) {
  for(int i = 0; i < n; ++i) {
        lines[i].clear();
      }
}

void ASCIINode::appendLines(std::string *lines, int n, const std::string text) {
  for(int i = 0; i < n; ++i) {
        lines[i] += text;
      }
}

void ASCIINode::outputLines(const std::string *lines, int n) {
  for (int i = 0; i < n; ++i) {
    std::cout << lines[i] << std::endl;
  }
}



