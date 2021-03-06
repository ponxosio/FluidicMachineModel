#ifndef SHORTSTATEPREDICATEGENERATOR_H
#define SHORTSTATEPREDICATEGENERATOR_H

#include <memory>
#include <stdexcept>
#include <vector>
#include <bitset>

#include <utils/Utils.h>

#include "fluidicmachinemodel/machinegraph.h"
#include "fluidicmachinemodel/machine_graph_utils/machinegraphpressureiterator.h"
#include "fluidicmachinemodel/machine_graph_utils/variablenominator.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/label.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/labelcombinationsiterator.h"
#include "fluidicmachinemodel/rules/rule.h"
#include "fluidicmachinemodel/rules/predicate.h"
#include "fluidicmachinemodel/rules/equality.h"
#include "fluidicmachinemodel/rules/arithmetic/arithmeticoperable.h"
#include "fluidicmachinemodel/rules/variabledomain.h"

class GraphRulesGenerator;
class CommomRulesOperations;

class ShortStatePredicateGenerator
{
    friend class GraphRulesGenerator;

    typedef std::unordered_map<std::tuple<int,int>, Label, PairIntIntHashFunction> LabelTypeTubeMap;
    typedef std::unordered_map<std::tuple<int,int>, TubeEdge::TubeDirection, PairIntIntHashFunction> TubeDirMap;

public:
    virtual ~ShortStatePredicateGenerator();

protected:
    const static Label dummy;

    std::shared_ptr<MachineGraph> graph;
    std::shared_ptr<CommomRulesOperations> calculator;
    std::shared_ptr<Rule> rule;

    ShortStatePredicateGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator) throw (std::invalid_argument);

    void analizeGraph();

    std::shared_ptr<Predicate> generatePredicateOpenContainer(int idOpenContainer) throw(std::invalid_argument);
    std::shared_ptr<Predicate> generatePredicateOpenContainer_moreOneTubeLeaving(int idOpenContainer);
    std::shared_ptr<Predicate> generatePredicateOpenContainer_moreOneTubeArriving(int idOpenContainer);
    std::shared_ptr<Predicate> generatePredicateOpenContainer_liquidId(int idOpenContainer);
    std::shared_ptr<Predicate> generatePredicateOpenContainer_arrivingLeavingCombination(int idOpenContainer,
                                                                                         bool arriving,
                                                                                         bool bothSigns,
                                                                                         const MachineGraph::GraphType::EdgeVector & tubesZero,
                                                                                         const MachineGraph::GraphType::EdgeVector & tubesNonZero);

    std::shared_ptr<Predicate> generatePredicateCloseContainer(int idCloseContainer) throw(std::invalid_argument);

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
    std::shared_ptr<Predicate> generatePredicateValve_processConnectedPins(int valveId,
                                                                           int possition,
                                                                           std::shared_ptr<ValveNode> valvePtr,
                                                                           LabelTypeTubeMap & arrivingMap,
                                                                           LabelTypeTubeMap & leavingMap) throw(std::invalid_argument);

    std::shared_ptr<Predicate> makeNodeTubesCombinations(int nodeId,
                                                         bool makeContainerEq,
                                                         const MachineGraph::GraphType::EdgeVector & tubesArriving,
                                                         const MachineGraph::GraphType::EdgeVector & tubesLeaving) throw(std::invalid_argument);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_makeIteratorsLeaving(int nodeId,
                                                                              bool makeContainerEq,
                                                                              const MachineGraph::GraphType::EdgeVector & arrivingDummy,
                                                                              const MachineGraph::GraphType::EdgeVector & leavingDummy,
                                                                              const LabelTypeTubeMap & leavingMap);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_makeIteratorsBoth(int nodeId,
                                                                           bool makeContainerEq,
                                                                           const MachineGraph::GraphType::EdgeVector & arrivingDummy,
                                                                           const LabelTypeTubeMap & arrivingMap,
                                                                           const MachineGraph::GraphType::EdgeVector & leavingDummy,
                                                                           const LabelTypeTubeMap & leavingMap);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_makeIteratorsArriving(int nodeId,
                                                                               bool makeContainerEq,
                                                                               const MachineGraph::GraphType::EdgeVector & arrivingDummy,
                                                                               const MachineGraph::GraphType::EdgeVector & leavingDummy,
                                                                               const LabelTypeTubeMap & arrivingMap);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_combinationNonZerosBoth(int nodeId,
                                                                                 bool makeContainerEq,
                                                                                 const LabelTypeTubeMap & labelsArriving,
                                                                                 const LabelTypeTubeMap & labelsLeaving,
                                                                                 const MachineGraph::GraphType::EdgeVector & dummyArriving,
                                                                                 const MachineGraph::GraphType::EdgeVector & dummyLeaving);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_combinationNonZerosLeaving(int nodeId,
                                                                                    bool makeContainerEq,
                                                                                    const LabelTypeTubeMap & labelsLeaving,
                                                                                    const MachineGraph::GraphType::EdgeVector & dummyArriving,
                                                                                    const MachineGraph::GraphType::EdgeVector & dummyLeaving);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_combinationNonZerosArriving(int nodeId,
                                                                                     bool makeContainerEq,
                                                                                     const LabelTypeTubeMap & labelsArriving,
                                                                                     const MachineGraph::GraphType::EdgeVector & dummyArriving,
                                                                                     const MachineGraph::GraphType::EdgeVector & dummyLeaving);

    std::shared_ptr<Predicate> makeNodeTubesCombinations_processCombination(int nodeId,
                                                                            bool makeContainerEq,
                                                                            const MachineGraph::GraphType::EdgeVector & aDummyBigger,
                                                                            const MachineGraph::GraphType::EdgeVector & aDummySmaller,
                                                                            const MachineGraph::GraphType::EdgeVector & lDummyBigger,
                                                                            const MachineGraph::GraphType::EdgeVector & lDummySmaller,
                                                                            const MachineGraph::GraphType::EdgeVector & aBigger,
                                                                            const MachineGraph::GraphType::EdgeVector & aSmaller,
                                                                            const MachineGraph::GraphType::EdgeVector & lBigger,
                                                                            const MachineGraph::GraphType::EdgeVector & lSmaller);

    bool checkBothDirection(const TubeDirMap & aDirMap,
                            const TubeDirMap & lDirMap,
                            const MachineGraph::GraphType::EdgeVector & aBigger,
                            const MachineGraph::GraphType::EdgeVector & aSmaller,
                            const MachineGraph::GraphType::EdgeVector &lBigger,
                            const MachineGraph::GraphType::EdgeVector &lSmaller);

    LabelTypeTubeMap labelNode(int node);
    LabelTypeTubeMap getSubMap(const LabelTypeTubeMap & labelMap, const MachineGraph::GraphType::EdgeVector & tubes) throw(std::invalid_argument);

    void labelNodeArrivingLeavingSet(int node, bool arriving, LabelTypeTubeMap & labelMap);
    void labelTubeSet(bool arriving,const MachineGraph::GraphType::EdgeVector & tubes, LabelTypeTubeMap & labelMap);
    Label labelTube(int nodeStart, int nodeAvoid);

    bool hasLabel(Label::LabelType type,
                  const LabelTypeTubeMap & labelMap,
                  const MachineGraph::GraphType::EdgeVector & tubes) throw(std::invalid_argument);
    bool hasLabel(Label::LabelType type, const LabelTypeTubeMap & labelMap);

    LabelTypeTubeMap makeLabelMap(const MachineGraph::GraphType::EdgeVector & tubes, const Label & initValue);
    void separateLabelMap(const LabelTypeTubeMap & map,
                          Label::LabelType type,
                          MachineGraph::GraphType::EdgeVector & tubesType,
                          MachineGraph::GraphType::EdgeVector & tubesNonType);
    void separateLabelMap(const LabelTypeTubeMap & map,
                          const Label & label,
                          MachineGraph::GraphType::EdgeVector & tubesType,
                          MachineGraph::GraphType::EdgeVector & tubesNonType);

    void subtractTubesWithoutLabel(const LabelTypeTubeMap & map,
                                   Label::LabelType type,
                                   MachineGraph::GraphType::EdgeVector & tubesToSubstract,
                                   MachineGraph::GraphType::EdgeVector & tubesToAddSubstracted) throw(std::invalid_argument);

    void separateDirMap(const TubeDirMap & dirMap,
                        MachineGraph::GraphType::EdgeVector & tubesRegular,
                        MachineGraph::GraphType::EdgeVector & tubesInverted);

    TubeDirMap makeTubeDirMap(const MachineGraph::GraphType::EdgeVector & tubes);
    bool canApplyDirArriving(Label::LabelType type, const TubeDirMap & arrivingMap, const TubeDirMap & leavingMap);
    bool canApplyDirLeaving(Label::LabelType type, const TubeDirMap & arrivingMap, const TubeDirMap & leavingMap);
    void applyDirArriving(Label::LabelType type, TubeDirMap & arrivingMap, TubeDirMap & leavingMap);
    void applyDirLeaving(Label::LabelType type, TubeDirMap & arrivingMap, TubeDirMap & leavingMap);



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
