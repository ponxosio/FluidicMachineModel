#include "shortstatepredicategenerator.h"

#include "machine_graph_utils/graphrulesgenerator.h"
#include "machine_graph_utils/rules_generation/commomrulesoperations.h"

ShortStatePredicateGenerator::ShortStatePredicateGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator) throw (std::invalid_argument)
{
    try {
        this->graph = graph;
        this->calculator = calculator;

        analizeGraph();
    } catch (std::exception & e) {
        throw(std::invalid_argument("ShortStatePredicateGenerator::ShortStatePredicateGenerator(). Error while analizing graph: " +
                                    graph->toString() + ". Error message" + std::string(e.what())));
    }
}

ShortStatePredicateGenerator::~ShortStatePredicateGenerator() {

}

void ShortStatePredicateGenerator::analizeGraph() {
    std::shared_ptr<Predicate> mainPred = NULL;

    for(int closeContainerId: graph->getCloseContainersIdsSet()) {
        std::shared_ptr<Predicate> closeContainerPred = generatePredicateCloseContainer(closeContainerId);
        mainPred = calculator->andPredicates(mainPred, closeContainerPred);
    }

    for(int openContainerId: graph->getOpenContainersIdsSet()) {
        std::shared_ptr<Predicate> openContainerPred = generatePredicateOpenContainer(openContainerId);
        mainPred = calculator->andPredicates(mainPred, openContainerPred);
    }

    for(int pumpId: graph->getPumpsIdsSet()) {
        std::shared_ptr<Predicate> pumpPred = generatePredicateOpenContainer(pumpId);
        mainPred = calculator->andPredicates(mainPred, pumpPred);
    }

    for(int valveId: graph->getValvesIdsSet()) {
        std::shared_ptr<Predicate> valvePred = generatePredicateValve(valveId);
        mainPred = calculator->andPredicates(mainPred, valvePred);
    }
    rules = mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer(int idOpenContainer) throw (std::invalid_argument){
    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(pumpId);
    MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(pumpId);

    if ((!leaving->empty() && arriving->empty()) ||
        (leaving->empty() && !arriving->empty()))
    {
        std::shared_ptr<Predicate> liquidIdConstarints = generatePredicateOpenContainer_liquidId(idOpenContainer);
        //TODO: combinaciones con 0 y calculo de rates.
    } else {
        throw(std::invalid_argument(std::to_string(idOpenContainer) + " openContainer must has only leaving or arriving tubes"));
    }

    return NULL;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer_liquidId(int idOpenContainer) {
    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(idOpenContainer);

    std::shared_ptr<Variable> containerVar = calculator->getVariable(VariableNominator::getContainerVarName(idOpenContainer));
    std::shared_ptr<Predicate> mainPred = calculator->createEquality(containerVarName, calculator->getNumber(0));

    std::shared_ptr<ArithmeticOperable> containerId = calculator->calculateId(containerVar);
    int liquidId = graph->getOpenContainerLiquidId(idOpenContainer);

    if (!leaving->empty()) {
        std::shared_ptr<Predicate> containerBig0 = calculator->createBigger(containerVarName, calculator->getNumber(0));
        std::shared_ptr<Predicate> idEq = calculator->createEquality(containerId, calculator->getNumber(liquidId));
        containerBig0 = calculator->andPredicates(containerBig0, idEq);

        std::shared_ptr<Predicate> containerLess0 = calculator->createLesser(containerVarName, calculator->getNumber(0));
        std::shared_ptr<Predicate> idNeq = calculator->createNotEqual(containerId, calculator->getNumber(liquidId));
        containerLess0 = calculator->andPredicates(containerLess0, idNeq);

        mainPred = calculator->orPredicates(mainPred, containerBig0);
        mainPred = calculator->orPredicates(mainPred, containerLess0);
    } else {
        std::shared_ptr<Predicate> containerBig0 = calculator->createBigger(containerVarName, calculator->getNumber(0));
        std::shared_ptr<Predicate> idNeq = calculator->createNotEqual(containerId, calculator->getNumber(liquidId));
        containerBig0 = calculator->andPredicates(containerBig0, idNeq);

        std::shared_ptr<Predicate> containerLess0 = calculator->createLesser(containerVarName, calculator->getNumber(0));
        std::shared_ptr<Predicate> idEq = calculator->createEquality(containerId, calculator->getNumber(liquidId));
        containerLess0 = calculator->andPredicates(containerLess0, idEq);

        mainPred = calculator->orPredicates(mainPred, containerBig0);
        mainPred = calculator->orPredicates(mainPred, containerLess0);
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateCloseContainer(int idCloseContainer) {
    return NULL;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicatePump(int pumpId) throw(std::invalid_argument) {
    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(pumpId);
    MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(pumpId);

    if (!leaving->empty() && !arriving->empty()) {
        std::shared_ptr<Predicate> mainPred = generatePredicatePump_pumpStop(pumpId);

        if (leaving->size() == 1 &&
            arriving->size() == 1)
        {
            //simple not neccesary to calculate combinations
            MachineGraph::GraphType::EdgeTypePtr leavingTube = *leaving->begin();
            MachineGraph::GraphType::EdgeTypePtr arrivingTube = *arriving->begin();
            std::shared_ptr<PumpNode> pumpPtr = graph->getPump(pumpId);

            std::shared_ptr<Predicate> regularDirection = generatePredicatePump_pumpStart(pumpId, 1, arrivingTube, Equality::bigger, *arriving.get(), *leaving.get());
            mainPred = calculator->orPredicates(mainPred, regularDirection);

            if (pumpPtr->getType() == PumpNode::bidirectional) {
                std::shared_ptr<Predicate> invertedDirection = generatePredicatePump_pumpStart(pumpId, -1, leavingTube, Equality::lesser, *leaving.get(), *arriving.get());
                mainPred = calculator->orPredicates(mainPred, invertedDirection);
            }
        } else {
            //comnplex
            std::shared_ptr<Predicate> complexStatesPredicates = generatePredicatePump_complex(pumpId);
            mainPred = calculator->orPredicates(mainPred, complexStatesPredicates);
        }
        return mainPred;
    } else {
        throw(std::invalid_argument("ShortStatePredicateGenerator::generatePredicatePump. " +
                                    std::to_string(pumpId) + " pump must has at least one arriving and one leaving tube"));
    }
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicatePump_pumpStop(int pumpId) {
    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(pumpId);
    MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(pumpId);

    std::shared_ptr<Predicate> pumpIsZero = calculator->createEquality(VariableNominator::getPumpVarName(pumpId), 0);
    std::shared_ptr<Predicate> rateIsZero = calculator->createEquality(VariableNominator::getPumpRateVarName(pumpId), 0);
    std::shared_ptr<Predicate> arrivingZero = allTubesSameEquality(*arriving.get(), Equality::equal, calculator->getNumber(0));
    std::shared_ptr<Predicate> leavingZero = allTubesSameEquality(*leaving.get(), Equality::equal, calculator->getNumber(0));

    std::shared_ptr<Predicate> mainPred = calculator->andPredicates(pumpIsZero, rateIsZero);
    mainPred = calculator->andPredicates(mainPred, arrivingZero);
    mainPred = calculator->andPredicates(mainPred, leavingZero);

    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicatePump_pumpStart(int pumpId,
                                                                                         int pumpState,
                                                                                         MachineGraph::GraphType::EdgeTypePtr rateEdge,
                                                                                         Equality::ComparatorOp op,
                                                                                         const MachineGraph::GraphType::EdgeVector & tubesArriving,
                                                                                         const MachineGraph::GraphType::EdgeVector & tubesLeaving)
{
    std::shared_ptr<Predicate> pumpIsState = calculator->createEquality(VariableNominator::getPumpVarName(pumpId), pumpState);

    std::shared_ptr<Predicate> rate = calculator->createEquality(VariableNominator::getPumpRateVarName(pumpId),
                                                                 VariableNominator::getTubeVarName(rateEdge->getIdSource(), rateEdge->getIdTarget()));

    std::shared_ptr<Predicate> arrivingSign = allTubesSameEquality(tubesArriving, op, calculator->getNumber(0));
    std::shared_ptr<Predicate> leavingSign = allTubesSameEquality(tubesLeaving, op, calculator->getNumber(0));

    std::shared_ptr<ArithmeticOperable> arrivingSum = addAllTubesProperty(tubesArriving, *calculator.get(), &CommomRulesOperations::calculateId);
    std::shared_ptr<Variable> arrivingRate = calculator->getVariable(VariableNominator::getTubeVarName(rateEdge->getIdSource(), rateEdge->getIdTarget()));
    arrivingSum = calculator->addOperables(arrivingSum, arrivingRate);

    std::shared_ptr<Predicate> leavingTubesEq = NULL;
    for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubesLeaving) {
        std::shared_ptr<Predicate> actualEq = calculator->createEquality(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()), arrivingSum);
        leavingTubesEq = calculator->andPredicates(leavingTubesEq, actualEq);
    }

    std::shared_ptr<Predicate> mainPred = calculator->andPredicates(pumpIsState, rate);
    mainPred = calculator->andPredicates(mainPred, arrivingSign);
    mainPred = calculator->andPredicates(mainPred, leavingSign);
    mainPred = calculator->andPredicates(mainPred, leavingTubesEq);

    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicatePump_complex(int pumpId) {
    std::shared_ptr<Predicate> mainPred = NULL;

    LabelTypeTubeMap labelMapArriving;
    labelNodeArrivingLeavingSet(pumpId, true, labelMapArriving);

    LabelTypeTubeMap labelMapLeaving;
    labelNodeArrivingLeavingSet(pumpId, false, labelMapLeaving);

    std::vector<Label::LabelType> types = {Label::zero};
    LabelCombinationsIterator arrivingZeroIt(labelMapArriving, types);
    LabelCombinationsIterator leavingZeroIt(labelMapLeaving, types);

    while(arrivingZeroIt.calculateNext()) {
        Label dummy;
        Label zero;
        zero.setLabel(Label::zero);

        LabelCombinationsIterator::LabelSet arrivinglabelSet = arrivingZeroIt.getLabelSet();
        if ((arrivinglabelSet.find(dummy) != arrivinglabelSet.end()))
        {
            while(leavingZeroIt.calculateNext()) {
                LabelCombinationsIterator::LabelSet leavingLabelSet = leavingZeroIt.getLabelSet();
                if ((leavingLabelSet.find(dummy) != arrivinglabelSet.end()))
                {
                    std::shared_ptr<Predicate> combinationPred = generatePredicatePump_allowedCombination(pumpId,
                                                                                                          arrivingZeroIt.getTubesLabelsMap(),
                                                                                                          leavingZeroIt.getTubesLabelsMap());
                    mainPred = calculator->orPredicates(mainPred, combinationPred);
                }
            }
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicatePump_allowedCombination(int pumpId,
                                                                                                  const LabelTypeTubeMap & arrivingMap,
                                                                                                  const LabelTypeTubeMap & leavingMap)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::array<MachineGraph::GraphType::EdgeVector, 2> arrivingZeroNonZero = separateLabelMap(arrivingMap, Label::zero);
    std::array<MachineGraph::GraphType::EdgeVector, 2> leavingZeroNonZero = separateLabelMap(leavingMap, Label::zero);

    const MachineGraph::GraphType::EdgeVector & arrivingNonZero = std::get<1>(arrivingZeroNonZero);
    const MachineGraph::GraphType::EdgeVector & arrivingZero = std::get<0>(arrivingZeroNonZero);

    const MachineGraph::GraphType::EdgeVector & leavingNonZero = std::get<1>(leavingZeroNonZero);
    const MachineGraph::GraphType::EdgeVector & leavingZero = std::get<0>(leavingZeroNonZero);

    std::shared_ptr<Predicate> tubesZeroA = allTubesSameEquality(arrivingZero, Equality::equal, calculator->getNumber(0));
    std::shared_ptr<Predicate> tubesZeroL = allTubesSameEquality(leavingZero, Equality::equal, calculator->getNumber(0));

    mainPred = calculator->andPredicates(tubesZeroA, tubesZeroL);

    std::shared_ptr<Predicate> tubesRateA = samePropertyTube(arrivingNonZero, *calculator.get(), &CommomRulesOperations::calculateRate);
    std::shared_ptr<Predicate> tubesRateL = samePropertyTube(leavingNonZero, *calculator.get(), &CommomRulesOperations::calculateRate);

    mainPred = calculator->andPredicates(mainPred, tubesRateA);
    mainPred = calculator->andPredicates(mainPred, tubesRateL);

    MachineGraph::GraphType::EdgeTypePtr edgeRate = *arrivingNonZero.begin();
    std::shared_ptr<Predicate> pumpRegular = generatePredicatePump_pumpStart(pumpId, 1, edgeRate, Equality::bigger, arrivingNonZero, leavingNonZero);

    std::shared_ptr<PumpNode> pumpPtr = graph->getPump(pumpId);
    if (pumpPtr->getType() == PumpNode::bidirectional) {
        MachineGraph::GraphType::EdgeTypePtr edgeRate = *leavingNonZero.begin();
        std::shared_ptr<Predicate> pumpInverted = generatePredicatePump_pumpStart(pumpId, -1, edgeRate, Equality::lesser, leavingNonZero, arrivingNonZero);
        pumpRegular = calculator->orPredicates(pumpRegular, pumpInverted);
    }

    mainPred = calculator->andPredicates(mainPred, pumpRegular);
    return mainPred;
}

std::array<MachineGraph::GraphType::EdgeVector, 2> ShortStatePredicateGenerator::separateLabelMap(const LabelTypeTubeMap & map, Label::LabelType type) {
    std::array<MachineGraph::GraphType::EdgeVector, 2> tubes;

    for(auto pair: map) {
        const std::tuple<int,int> & tubeId = pair.first;
        Label label = pair.second;

        if (label.hasLabelMask(type)) {
            MachineGraph::GraphType::EdgeTypePtr tube = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));
            std::get<0>(tubes).push_back(tube);
        } else {
            MachineGraph::GraphType::EdgeTypePtr tube = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));
            std::get<1>(tubes).push_back(tube);
        }
    }
    return tubes;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateValve(int valveId) {
    return NULL;
}

ShortStatePredicateGenerator::LabelTypeTubeMap ShortStatePredicateGenerator::labelNode(int node) {
    LabelTypeTubeMap labelMap;

    labelNodeArrivingLeavingSet(node, true, labelMap);
    labelNodeArrivingLeavingSet(node, false, labelMap);

    return labelMap;
}

void ShortStatePredicateGenerator::labelNodeArrivingLeavingSet(int node, bool arriving, LabelTypeTubeMap & labelMap) {
    MachineGraph::GraphType::EdgeVectorPtr tubes = NULL;
    if (arriving) {
        tubes = graph->getArrivingTubes(node);
    } else {
        tubes = graph->getLeavingTubes(node);
    }

    if (arriving) {
        for(const MachineGraph::GraphType::EdgeTypePtr & tube: *tubes.get()) {
            Label tubeLabel = labelTube(tube->getIdSource(), tube->getIdTarget());
            labelMap.insert(std::make_pair(std::make_tuple(tube->getIdSource(), tube->getIdTarget()), tubeLabel));
        }
    } else {
        for(const MachineGraph::GraphType::EdgeTypePtr & tube: *tubes.get()) {
            Label tubeLabel = labelTube(tube->getIdTarget(), tube->getIdSource());
            labelMap.insert(std::make_pair(std::make_tuple(tube->getIdSource(), tube->getIdTarget()), tubeLabel));
        }
    }
}

Label ShortStatePredicateGenerator::labelTube(int nodeStart, int nodeAvoid) {
    Label tubeLbel;

    std::unordered_set<int> nodes2avoid = {nodeAvoid};
    MachineGraphIterator graphIt(nodeStart, graph, nodes2avoid);

    while(graphIt.advance() && !tubeLbel.isFull()) {
        int nextNode = graphIt.getActualNode();

        if (graph->isValve(nextNode)) {
            tubeLbel.setLabel(Label::zero);
        } else if(graph->isPump(nextNode)) {
            tubeLbel.setLabel(Label::zero);
            tubeLbel.setLabel(Label::bigger);

            std::shared_ptr<PumpNode> pumpPtr = graph->getPump(nextNode);
            if (pumpPtr->getType() == PumpNode::bidirectional) {
                tubeLbel.setLabel(Label::smaller);
            }
        }
    }
    return tubeLbel;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::allTubesSameEquality(const MachineGraph::GraphType::EdgeVector & tubes,
                                                                              Equality::ComparatorOp op,
                                                                              std::shared_ptr<ArithmeticOperable> comparator)
{
    std::shared_ptr<Predicate> mainPred = NULL;
    for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubes)  {
        std::shared_ptr<ArithmeticOperable> varTube = calculator->getVariable(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()));
        std::shared_ptr<Predicate> eqPred = std::make_shared<Equality>(varTube, op, comparator);

        mainPred = calculator->andPredicates(mainPred, eqPred);
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::samePropertyTube(const MachineGraph::GraphType::EdgeVector & tubes,
                                                                                     CommomRulesOperations & operations,
                                                                                     std::shared_ptr<ArithmeticOperable> (CommomRulesOperations::*propertyCalculator)(std::shared_ptr<Variable>))
{
    std::shared_ptr<Predicate> mainPred = NULL;
    if (tubes.size() > 1) {
        auto it = tubes.begin();
        MachineGraph::GraphType::EdgeTypePtr firstTube = *it;
        std::shared_ptr<Variable> firstTubeVar = calculator->getVariable(VariableNominator::getTubeVarName(firstTube->getIdSource(), firstTube->getIdTarget()));
        std::shared_ptr<ArithmeticOperable> firstRate = calculator->calculateRate(firstTubeVar);
        ++it;

        for(; it != tubes.end(); ++it) {
            MachineGraph::GraphType::EdgeTypePtr nextTube = *it;
            std::shared_ptr<Variable> nextTubeVar = calculator->getVariable(VariableNominator::getTubeVarName(nextTube->getIdSource(), nextTube->getIdTarget()));
            std::shared_ptr<ArithmeticOperable> nextRate = (operations.*propertyCalculator)(nextTubeVar);

            std::shared_ptr<Predicate> tempPred = calculator->createEquality(firstRate, nextRate);
            mainPred = calculator->andPredicates(mainPred, tempPred);

            firstRate = nextRate;
        }
    }
    return mainPred;
}

std::shared_ptr<ArithmeticOperable> ShortStatePredicateGenerator::addAllTubesProperty(const MachineGraph::GraphType::EdgeVector & tubes,
                                                                                      CommomRulesOperations & operations,
                                                                                      std::shared_ptr<ArithmeticOperable> (CommomRulesOperations::*propertyCalculator)(std::shared_ptr<Variable>))
{
    std::shared_ptr<ArithmeticOperable> mainOp = NULL;

    for(const MachineGraph::GraphType::EdgeTypePtr & nextTube: tubes) {
        std::shared_ptr<Variable> nextTubeVar = calculator->getVariable(VariableNominator::getTubeVarName(nextTube->getIdSource(), nextTube->getIdTarget()));
        std::shared_ptr<ArithmeticOperable> nextOp = (operations.*propertyCalculator)(nextTubeVar);

        mainOp = calculator->addOperables(mainOp, nextOp);
    }
    return mainOp;
}




















