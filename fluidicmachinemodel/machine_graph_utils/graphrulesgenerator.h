
#ifndef GRAPHRULESGENERATOR_H
#define GRAPHRULESGENERATOR_H

#define LONG_LONG_DIGITS_SIZE 18

#include <memory>
#include <vector>
#include <stdexcept>

#include "fluidicmachinemodel/machinegraph.h"
#include "fluidicmachinemodel/rules/rule.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class GRAPHRULESGENERATOR_EXPORT GraphRulesGenerator
{

public:
    GraphRulesGenerator(std::shared_ptr<MachineGraph> graph,
                        short int ratePrecisionInteger,
                        short int ratePrecisionDecimal) throw(std::invalid_argument);
    virtual ~GraphRulesGenerator();

    inline const std::vector<std::shared_ptr<Rule>> & getRules() {
        return rules;
    }

protected:
    std::vector<std::shared_ptr<Rule>> rules;
};

#endif // GRAPHRULESGENERATOR_H
