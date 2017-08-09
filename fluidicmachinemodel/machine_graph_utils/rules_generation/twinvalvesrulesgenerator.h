#ifndef TWINVALVESRULESGENERATOR_H
#define TWINVALVESRULESGENERATOR_H

#include <memory>
#include <unordered_set>
#include <vector>

#include "fluidicmachinemodel/machine_graph_utils/variablenominator.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/commomrulesoperations.h"
#include "fluidicmachinemodel/rules/rule.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class TwinValvesRulesGenerator
{
public:
    static std::shared_ptr<Rule> generateTwinValvesRules(const std::vector<std::unordered_set<int>> & twinValves, std::shared_ptr<CommomRulesOperations> crOperator);
};

#endif // TWINVALVESRULESGENERATOR_H
