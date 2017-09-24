//
// Created by msagebaum on 9/24/17.
//

#ifndef FACTORIOVIEWER_FACTORIO_H
#define FACTORIOVIEWER_FACTORIO_H

#include <string>
#include <vector>

struct Part {
  std::string name;
  double quantity;
  std::string type;

  Part() :
      name(),
      quantity(0),
      type("item") {}
};

struct Recipe {
  std::string name;
  std::string creates;
  std::vector<Part> parts;
  std::vector<Part> results;
  double time;
  double yield;
  double fabSpeed;

  Recipe() :
      name(),
      creates(),
      parts(),
      time(0.5),
      yield(1),
      fabSpeed(1.0) {}

  void finish() {
    if(0 == this->results.size()) {
      Part part;
      part.name = this->creates;
      part.quantity = this->yield;
      this->results.push_back(part);
    }
  }

  double normalizeAmount(const std::string& item, const double& amount) const {
    double amountNormalized = amount;

    for(const Part& part : results) {
      if(0 == part.name.compare(item)) {
        amountNormalized /= part.quantity;
      }
    }

    return amountNormalized;
  }
};


#endif //FACTORIOVIEWER_FACTORIO_H
