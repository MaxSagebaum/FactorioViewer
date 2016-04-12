//
// Created by Niemand on 4/10/16.
//

#ifndef FACTORIOVIEWER_ASCIITREE_H
#define FACTORIOVIEWER_ASCIITREE_H

#include <vector>
#include <string>

struct ASCIINode {
  std::vector<std::string> lines;
  int maxSize;
  std::vector<ASCIINode> children;

  int pos;

  ASCIINode();

  void addLine(const std::string& line);

  void addChild(const ASCIINode& node);

  void printTree();

  static void printList(const std::vector<ASCIINode>& nodes, int maxSize = 5);

  static void outputLines(const std::string *lines, int n);

  static void appendLines(std::string *lines, int n, const std::string text);

  static void clearLines(std::string *lines, const int n);
};


#endif //FACTORIOVIEWER_ASCIITREE_H
