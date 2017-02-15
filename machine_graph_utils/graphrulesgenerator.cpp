#include "graphrulesgenerator.h"
#include "machine_graph_utils/rules_generation/machinegraphgenerator.h"
#include "machine_graph_utils/rules_generation/commomrulesoperations.h"
#include "machine_graph_utils/rules_generation/domaingenerator.h"
#include "machine_graph_utils/rules_generation/statepredicategenerator.h"

GraphRulesGenerator::GraphRulesGenerator(std::shared_ptr<MachineGraph> graph,
                                        short int ratePrecisionInteger,
                                        short int ratePrecisionDecimal)
                throw(std::invalid_argument)
{
    try {
        std::shared_ptr<CommomRulesOperations> calculator(new CommomRulesOperations(ratePrecisionInteger, ratePrecisionDecimal));

        DomainGenerator domains(graph, calculator);
        StatePredicateGenerator predicates(graph, calculator);

        const std::vector<std::shared_ptr<Rule>> & domainRules = domains.getDomainsRules();
        const std::vector<std::shared_ptr<Rule>> & predicatesRules = predicates.rules;

        rules.reserve(domainRules.size() + predicatesRules.size());
        rules.insert(rules.begin(), domainRules.begin(), domainRules.end());
        rules.insert(rules.begin() + domainRules.size(), predicatesRules.begin(), predicatesRules.end());
    } catch (std::exception & e) {
        throw(std::invalid_argument("Impossible to generate rules for graph: " + graph->toString() +
                                    ".Error message: " + std::string(e.what())));
    }
}

GraphRulesGenerator::~GraphRulesGenerator() {
}

