#include "twinvalvesrulesgenerator.h"

std::shared_ptr<Rule> TwinValvesRulesGenerator::generateTwinValvesRules(
        const std::vector<std::unordered_set<int>> & twinValves,
        std::shared_ptr<CommomRulesOperations> crOperator)
{
    std::shared_ptr<Predicate> mainRule = NULL;
    for(const std::unordered_set<int> & twins : twinValves) {

        std::shared_ptr<Predicate> twinValveRule = NULL;

        auto it = twins.begin();
        int lastTwinId = *it;
        ++it;

        for(; it != twins.end(); ++it) {
            int actualTwinId = *it;

            std::string lastTwinStr = VariableNominator::getValveVarName(lastTwinId);
            std::string actualTwinStr = VariableNominator::getValveVarName(actualTwinId);

            std::shared_ptr<Predicate> actualEq = crOperator->createEquality(crOperator->getVariable(lastTwinStr),crOperator->getVariable(actualTwinStr));
            twinValveRule = crOperator->andPredicates(twinValveRule, actualEq);

            lastTwinId = actualTwinId;
        }
        mainRule = crOperator->andPredicates(mainRule, twinValveRule);
    }
    return mainRule;
}
