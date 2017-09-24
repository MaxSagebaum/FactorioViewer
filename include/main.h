//
// Created by msagebaum on 9/24/17.
//

#ifndef FACTORIOVIEWER_MAIN_H
#define FACTORIOVIEWER_MAIN_H

#include "Settings.h"
#include "factorio.h"
#include "ProductionNode.h"

void addItem(const Recipe &recipe, double amountNormalized, ProductionNode &node, TotalList& totals, const Settings& settings, const std::string& target);
void addItem(const Part &part, double amount, ProductionNode &node, TotalList& totals, const Settings& settings);

#endif //FACTORIOVIEWER_MAIN_H
