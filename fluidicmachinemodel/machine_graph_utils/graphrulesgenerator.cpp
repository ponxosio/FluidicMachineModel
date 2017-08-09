#include "graphrulesgenerator.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/commomrulesoperations.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/domaingenerator.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/shortstatepredicategenerator.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/twinvalvesrulesgenerator.h"

GraphRulesGenerator::GraphRulesGenerator(std::shared_ptr<MachineGraph> graph,
                                        short int ratePrecisionInteger,
                                        short int ratePrecisionDecimal)
                throw(std::invalid_argument)
{
    try {
        std::shared_ptr<CommomRulesOperations> calculator(new CommomRulesOperations(ratePrecisionInteger, ratePrecisionDecimal));

        DomainGenerator domains(graph, calculator);
        ShortStatePredicateGenerator predicates(graph, calculator);

        std::shared_ptr<Rule> twinValvesRules = TwinValvesRulesGenerator::generateTwinValvesRules(graph->getTwinsValves(), calculator);
        std::shared_ptr<Rule> allRules = calculator->andPredicates(std::dynamic_pointer_cast<Predicate>(twinValvesRules), std::dynamic_pointer_cast<Predicate>(predicates.rule));

        const std::vector<std::shared_ptr<Rule>> & domainRules = domains.getDomainsRules();

        rules.reserve(domainRules.size() + 1);
        rules.insert(rules.begin(), domainRules.begin(), domainRules.end());
        rules.insert(rules.begin() + domainRules.size(), allRules);
    } catch (std::exception & e) {
        throw(std::invalid_argument("Impossible to generate rules for graph: " + graph->toString() +
                                    ".Error message: " + std::string(e.what())));
    }
}

GraphRulesGenerator::~GraphRulesGenerator() {
}

