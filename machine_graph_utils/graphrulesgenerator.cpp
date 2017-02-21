#include "graphrulesgenerator.h"
#include "machine_graph_utils/rules_generation/commomrulesoperations.h"
#include "machine_graph_utils/rules_generation/domaingenerator.h"
#include "machine_graph_utils/rules_generation/shortstatepredicategenerator.h"

GraphRulesGenerator::GraphRulesGenerator(std::shared_ptr<MachineGraph> graph,
                                        short int ratePrecisionInteger,
                                        short int ratePrecisionDecimal)
                throw(std::invalid_argument)
{
    try {
        std::shared_ptr<CommomRulesOperations> calculator(new CommomRulesOperations(ratePrecisionInteger, ratePrecisionDecimal));

        DomainGenerator domains(graph, calculator);
        ShortStatePredicateGenerator predicates(graph, calculator);

        const std::vector<std::shared_ptr<Rule>> & domainRules = domains.getDomainsRules();

        rules.reserve(domainRules.size() + 1);
        rules.insert(rules.begin(), domainRules.begin(), domainRules.end());
        rules.insert(rules.begin() + domainRules.size(), predicates.rule);
    } catch (std::exception & e) {
        throw(std::invalid_argument("Impossible to generate rules for graph: " + graph->toString() +
                                    ".Error message: " + std::string(e.what())));
    }
}

GraphRulesGenerator::~GraphRulesGenerator() {
}

