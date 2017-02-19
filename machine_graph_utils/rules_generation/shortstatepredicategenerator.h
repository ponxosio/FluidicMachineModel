#ifndef SHORTSTATEPREDICATEGENERATOR_H
#define SHORTSTATEPREDICATEGENERATOR_H

#include <array>
#include <memory>
#include <stdexcept>
#include <vector>
#include <bitset>

#include "machinegraph.h"
#include "machine_graph_utils/machinegraphiterator.h"
#include "machine_graph_utils/variablenominator.h"
#include "machine_graph_utils/rules_generation/label.h"
#include "machine_graph_utils/rules_generation/labelcombinationsiterator.h"
#include "rules/rule.h"
#include "rules/predicate.h"
#include "rules/equality.h"
#include "rules/arithmetic/arithmeticoperable.h"
#include "rules/variabledomain.h"
#include "util/Utils.h"

class GraphRulesGenerator;
class CommomRulesOperations;

class ShortStatePredicateGenerator
{
    friend class GraphRulesGenerator;

    typedef std::unordered_map<std::tuple<int,int>, Label, PairIntIntHashFunction> LabelTypeTubeMap;

public:
    virtual ~ShortStatePredicateGenerator();

protected:
    std::shared_ptr<MachineGraph> graph;
    std::shared_ptr<CommomRulesOperations> calculator;
    std::shared_ptr<Rule> rules;

    ShortStatePredicateGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator) throw (std::invalid_argument);

    void analizeGraph();

    std::shared_ptr<Predicate> generatePredicateOpenContainer(int idOpenContainer) throw(std::invalid_argument);
    std::shared_ptr<Predicate> generatePredicateOpenContainer_liquidId(int idOpenContainer);

    std::shared_ptr<Predicate> generatePredicateCloseContainer(int idCloseContainer);

    std::shared_ptr<Predicate> generatePredicatePump(int pumpId) throw(std::invalid_argument);
    std::shared_ptr<Predicate> generatePredicatePump_pumpStop(int pumpId);
    std::shared_ptr<Predicate> generatePredicatePump_complex(int pumpId);
    std::shared_ptr<Predicate> generatePredicatePump_allowedCombination(int pumpId, const LabelTypeTubeMap & arrivingMap, const LabelTypeTubeMap & leavingMap);
    std::shared_ptr<Predicate> generatePredicatePump_pumpStart(int pumpId,
                                                               int pumpState,
                                                               MachineGraph::GraphType::EdgeTypePtr rateEdge,
                                                               Equality::ComparatorOp op,
                                                               const MachineGraph::GraphType::EdgeVector & tubesArriving,
                                                               const MachineGraph::GraphType::EdgeVector & tubesLeaving);

    std::shared_ptr<Predicate> generatePredicateValve(int valveId);

    LabelTypeTubeMap labelNode(int node);
    void labelNodeArrivingLeavingSet(int node, bool arriving, LabelTypeTubeMap & labelMap);
    Label labelTube(int nodeStart, int nodeAvoid);

    std::array<MachineGraph::GraphType::EdgeVector, 2> separateLabelMap(const LabelTypeTubeMap & map, Label::LabelType type);

    std::shared_ptr<Predicate> samePropertyTube(const MachineGraph::GraphType::EdgeVector & tubes,
                                                           CommomRulesOperations & operations,
                                                           std::shared_ptr<ArithmeticOperable> (CommomRulesOperations::*propertyCalculator)(std::shared_ptr<Variable>));

    std::shared_ptr<ArithmeticOperable> addAllTubesProperty(const MachineGraph::GraphType::EdgeVector & tubes,
                                                           CommomRulesOperations & operations,
                                                           std::shared_ptr<ArithmeticOperable> (CommomRulesOperations::*propertyCalculator)(std::shared_ptr<Variable>));

    std::shared_ptr<Predicate> allTubesSameEquality(const MachineGraph::GraphType::EdgeVector & tubes,
                                                    Equality::ComparatorOp op,
                                                    std::shared_ptr<ArithmeticOperable> comparator);

};

#endif // SHORTSTATEPREDICATEGENERATOR_H
