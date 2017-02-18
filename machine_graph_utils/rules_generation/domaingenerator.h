#ifndef DOMAINGENERATOR_H
#define DOMAINGENERATOR_H

#include <memory>
#include <stdexcept>
#include <vector>

#include "machinegraph.h"
#include "machine_graph_utils/variablenominator.h"
#include "rules/rule.h"
#include "rules/variabledomain.h"

class GraphRulesGenerator;
class CommomRulesOperations;

class DomainGenerator
{
    friend class GraphRulesGenerator;
public:
    virtual ~DomainGenerator();

protected:
    std::shared_ptr<MachineGraph> graph;
    std::shared_ptr<CommomRulesOperations> calculator;
    std::vector<std::shared_ptr<Rule>> rules;

    DomainGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator) throw(std::invalid_argument);

    void generateDomain();
    std::shared_ptr<VariableDomain> generateDomainPumpDir(int id);
    std::shared_ptr<VariableDomain> generateDomainPumpRate(int id);
    std::shared_ptr<VariableDomain> generateDomainValve(int id) throw(std::invalid_argument);
    std::shared_ptr<VariableDomain> generateDomainCloseContainer(int id);
    std::shared_ptr<VariableDomain> generateDomainOpenContainer(int id);
    std::shared_ptr<VariableDomain> generateDomainTube(int idSource, int idTarget);

    std::set<int> getValvePossibleValues(int id) throw(std::invalid_argument);
    std::vector<std::shared_ptr<Variable>> generateVariablesVector(const MachineGraph::GraphType::EdgeVector & edges);
    std::vector<VariableDomain::DomainTuple> generateDomainForValues(const std::vector<long long> & values) throw(std::invalid_argument);

    inline const std::vector<std::shared_ptr<Rule>> & getDomainsRules() {
        return rules;
    }
};

#endif // DOMAINGENERATOR_H
