#ifndef STATEPREDICATEGENERATOR_H
#define STATEPREDICATEGENERATOR_H

#include <memory>
#include <stdexcept>
#include <vector>

#include "machinegraph.h"
#include "machine_graph_utils/variablenominator.h"
#include "machine_graph_utils/rules_generation/machinegraphgenerator.h"
#include "rules/rule.h"
#include "rules/predicate.h"
#include "rules/arithmetic/arithmeticoperable.h"
#include "rules/variabledomain.h"

class GraphRulesGenerator;
class CommomRulesOperations;

class StatePredicateGenerator
{
    friend class GraphRulesGenerator;

public:
    virtual ~StatePredicateGenerator();

protected:
    std::shared_ptr<MachineGraph> graph;
    std::shared_ptr<CommomRulesOperations> calculator;
    std::vector<std::shared_ptr<Rule>> rules;

    StatePredicateGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator) throw (std::invalid_argument);

    std::shared_ptr<Predicate> generatePumpsCloseState();

    std::shared_ptr<Predicate> generateValidStates() throw(std::invalid_argument);
    std::shared_ptr<Predicate> generatePredicateActualState(MachineGraphGenerator & generator);
    std::shared_ptr<Predicate> generatePredicateConnectedComponent(std::shared_ptr<MachineGraph> component) throw(std::invalid_argument);

    std::shared_ptr<Predicate> generatePredicateSameRateTube(const MachineGraph::GraphType::EdgeVector & arriving);
    std::shared_ptr<Predicate> generatePredicateCloseContainer(int nodeId, std::shared_ptr<ArithmeticOperable> arrivingSum);
    std::shared_ptr<Predicate> generatePredicateOpenContainer(int nodeId, std::shared_ptr<MachineGraph> component);
    std::shared_ptr<Predicate> generatePredicatePump(int pumpId,
                                                     const MachineGraph::GraphType::EdgeVector & leaving,
                                                     const MachineGraph::GraphType::EdgeVector & arriving);

    std::shared_ptr<Predicate> calculateLeavingTubeValue(const MachineGraph::GraphType::EdgeVector & leaving, std::shared_ptr<ArithmeticOperable> arrivingSum);
    std::shared_ptr<ArithmeticOperable> calculatedVectorTubeSum(const MachineGraph::GraphType::EdgeVector & vector);
    std::shared_ptr<Predicate> generatePredicateConnectedComponent_processTubes(int nodeId,
                                           std::shared_ptr<MachineGraph> graph,
                                           MachineGraph::GraphType::EdgeVector & leaving,
                                           MachineGraph::GraphType::EdgeVector & arriving);

    std::set<int> getValvePossibleValues(int id) throw(std::invalid_argument);
};

#endif // STATEPREDICATEGENERATOR_H
