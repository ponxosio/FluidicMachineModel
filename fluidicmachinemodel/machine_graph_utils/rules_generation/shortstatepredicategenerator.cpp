#include "shortstatepredicategenerator.h"

#include "fluidicmachinemodel/machine_graph_utils/graphrulesgenerator.h"
#include "fluidicmachinemodel/machine_graph_utils/rules_generation/commomrulesoperations.h"

const Label ShortStatePredicateGenerator::dummy;

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
        std::shared_ptr<Predicate> pumpPred = generatePredicatePump(pumpId);
        mainPred = calculator->andPredicates(mainPred, pumpPred);
    }

    for(int valveId: graph->getValvesIdsSet()) {
        std::shared_ptr<Predicate> valvePred = generatePredicateValve(valveId);
        mainPred = calculator->andPredicates(mainPred, valvePred);
    }
    rule = mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer(int idOpenContainer) throw (std::invalid_argument){
    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(idOpenContainer);
    MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(idOpenContainer);

    if ((!leaving->empty() && arriving->empty()) ||
        (leaving->empty() && !arriving->empty()))
    {
        std::shared_ptr<Predicate> mainPred = generatePredicateOpenContainer_liquidId(idOpenContainer);

        std::shared_ptr<Predicate> combinationsPred = NULL;
        if (!leaving->empty()) { //all tubes leaving
            if (leaving->size() == 1) {
                MachineGraph::GraphType::EdgeTypePtr tube = leaving->back();
                std::shared_ptr<Variable> tubeVar = calculator->getVariable(
                            VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()));

                combinationsPred = calculator->createEquality(VariableNominator::getContainerVarName(idOpenContainer),
                                                              calculator->changeSign(tubeVar));
            } else {
                combinationsPred = generatePredicateOpenContainer_moreOneTubeLeaving(idOpenContainer);
            }
        } else { //all tubes arriving
            if (arriving->size() == 1) {
                MachineGraph::GraphType::EdgeTypePtr tube = arriving->back();
                std::shared_ptr<Variable> tubeVar = calculator->getVariable(
                            VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()));

                combinationsPred = calculator->createEquality(VariableNominator::getContainerVarName(idOpenContainer), tubeVar);
            } else {
                combinationsPred = generatePredicateOpenContainer_moreOneTubeArriving(idOpenContainer);
            }
        }

        mainPred = calculator->andPredicates(mainPred, combinationsPred);
        return mainPred;
    } else {
        throw(std::invalid_argument("ShortStatePredicateGenerator::generatePredicateOpenContainer." + std::to_string(idOpenContainer) + " openContainer must has only leaving or arriving tubes"));
    }
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer_moreOneTubeLeaving(int idOpenContainer) {
    std::shared_ptr<Predicate> mainPred = NULL;

    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(idOpenContainer);

    LabelTypeTubeMap labelMap;
    labelNodeArrivingLeavingSet(idOpenContainer, false, labelMap);
    bool bothSigns = hasLabel(Label::smaller, labelMap);

    std::vector<Label::LabelType> labelTypes = {Label::zero};
    LabelCombinationsIterator labelIt(labelMap, labelTypes);
    while(labelIt.calculateNext()) {
        std::shared_ptr<Predicate> actualCombination;

        Label dummy;
        const LabelCombinationsIterator::LabelSet & actualLabelSet = labelIt.getLabelSet();
        if (actualLabelSet.find(dummy) == actualLabelSet.end()) {
            //all leaving tubes are 0
            std::shared_ptr<Predicate> allTubesZero = allTubesSameEquality(*leaving.get(), Equality::equal, calculator->getNumber(0));
            std::shared_ptr<Predicate> containerEq0 = calculator->createEquality(VariableNominator::getContainerVarName(idOpenContainer), 0);
            actualCombination = calculator->andPredicates(allTubesZero, containerEq0);
        } else {
            //at least one tube is not 0
            MachineGraph::GraphType::EdgeVector tubesZero;
            MachineGraph::GraphType::EdgeVector tubesNonZero;

            separateLabelMap(labelIt.getTubesLabelsMap(), Label::zero, tubesZero, tubesNonZero);
            actualCombination = generatePredicateOpenContainer_arrivingLeavingCombination(idOpenContainer, false, bothSigns, tubesZero, tubesNonZero);
        }
        mainPred = calculator->orPredicates(mainPred, actualCombination);
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer_moreOneTubeArriving(int idOpenContainer) {
    std::shared_ptr<Predicate> mainPred = NULL;

     MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(idOpenContainer);

    LabelTypeTubeMap labelMap;
    labelNodeArrivingLeavingSet(idOpenContainer, true, labelMap);
    bool bothSigns = hasLabel(Label::smaller, labelMap);

    std::vector<Label::LabelType> labelTypes = {Label::zero};
    LabelCombinationsIterator labelIt(labelMap, labelTypes);
    while(labelIt.calculateNext()) {
        std::shared_ptr<Predicate> actualCombination;

        Label dummy;
        const LabelCombinationsIterator::LabelSet & actualLabelSet = labelIt.getLabelSet();
        if (actualLabelSet.find(dummy) == actualLabelSet.end()) {
            //all leaving tubes are 0
            std::shared_ptr<Predicate> allTubesZero = allTubesSameEquality(*arriving.get(), Equality::equal, calculator->getNumber(0));
            std::shared_ptr<Predicate> containerEq0 = calculator->createEquality(VariableNominator::getContainerVarName(idOpenContainer), 0);
            actualCombination = calculator->andPredicates(allTubesZero, containerEq0);
        } else {
            //at least one tube is not 0
            MachineGraph::GraphType::EdgeVector tubesZero;
            MachineGraph::GraphType::EdgeVector tubesNonZero;

            separateLabelMap(labelIt.getTubesLabelsMap(), Label::zero, tubesZero, tubesNonZero);
            actualCombination = generatePredicateOpenContainer_arrivingLeavingCombination(idOpenContainer, true, bothSigns, tubesZero, tubesNonZero);
        }
        mainPred = calculator->orPredicates(mainPred, actualCombination);
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer_arrivingLeavingCombination(
        int idOpenContainer,
        bool arriving,
        bool bothSigns,
        const MachineGraph::GraphType::EdgeVector & tubesZero,
        const MachineGraph::GraphType::EdgeVector & tubesNonZero)
{
    std::shared_ptr<Predicate> mainPred = allTubesSameEquality(tubesZero, Equality::equal, calculator->getNumber(0));

    std::shared_ptr<Predicate> bgZero = allTubesSameEquality(tubesNonZero, Equality::bigger, calculator->getNumber(0));

    std::shared_ptr<Predicate> smZero = NULL;
    if (bothSigns) {
        smZero = allTubesSameEquality(tubesNonZero, Equality::lesser, calculator->getNumber(0));
    }

    std::shared_ptr<Variable> containerVar = calculator->getVariable(VariableNominator::getContainerVarName(idOpenContainer));

    std::shared_ptr<ArithmeticOperable> idSum = addAllTubesProperty(tubesNonZero, *calculator.get(), &CommomRulesOperations::calculateId);
    std::shared_ptr<ArithmeticOperable> rateSum = addAllTubesProperty(tubesNonZero, *calculator.get(), &CommomRulesOperations::calculateRate);
    std::shared_ptr<ArithmeticOperable> sumIdSumRate = calculator->addOperables(idSum, rateSum);

    std::shared_ptr<Predicate> sameId = samePropertyTube(tubesNonZero, *calculator.get(), &CommomRulesOperations::calculateId);

    MachineGraph::GraphType::EdgeTypePtr tube0 = *tubesNonZero.begin();
    std::shared_ptr<Variable> tube0id = calculator->getVariable(VariableNominator::getTubeVarName(tube0->getIdSource(), tube0->getIdTarget()));
    std::shared_ptr<ArithmeticOperable> idSumRate = calculator->addOperables(calculator->calculateId(tube0id), rateSum);

    std::shared_ptr<Predicate> nonZeroPredicate = NULL;
    if (arriving) {
        std::shared_ptr<Predicate> containerPositiveEq = calculator->createEquality(containerVar, sumIdSumRate);
        std::shared_ptr<Predicate> containerNegativeEq = calculator->createEquality(containerVar, idSumRate);

        bgZero = calculator->andPredicates(bgZero, containerPositiveEq);

        if (bothSigns) {
            smZero = calculator->andPredicates(smZero, sameId);
            smZero = calculator->andPredicates(smZero, containerNegativeEq);
        }

        nonZeroPredicate = calculator->orPredicates(bgZero, smZero);
    } else {
        sumIdSumRate = calculator->changeSign(sumIdSumRate);
        idSumRate = calculator->changeSign(idSumRate);

        std::shared_ptr<Predicate> containerPositiveEq = calculator->createEquality(containerVar, idSumRate);
        std::shared_ptr<Predicate> containerNegativeEq = calculator->createEquality(containerVar, sumIdSumRate);

        bgZero = calculator->andPredicates(bgZero, sameId);
        bgZero = calculator->andPredicates(bgZero, containerPositiveEq);

        if (bothSigns) {
            smZero = calculator->andPredicates(smZero, containerNegativeEq);
        }

        nonZeroPredicate = calculator->orPredicates(bgZero, smZero);
    }
    mainPred = calculator->andPredicates(mainPred, nonZeroPredicate);
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateOpenContainer_liquidId(int idOpenContainer) {
    std::shared_ptr<Variable> containerVar = calculator->getVariable(VariableNominator::getContainerVarName(idOpenContainer));
    std::shared_ptr<Predicate> mainPred = calculator->createEquality(containerVar, calculator->getNumber(0));

    std::shared_ptr<ArithmeticOperable> containerId = calculator->calculateIdNotPower(containerVar);
    int liquidId = 1 << graph->getOpenContainerLiquidId(idOpenContainer);


    std::shared_ptr<Predicate> containerBig0 = calculator->createBigger(containerVar, calculator->getNumber(0));
    std::shared_ptr<Predicate> idEq = calculator->createNotEqual(containerId, calculator->getNumber(liquidId));
    containerBig0 = calculator->andPredicates(containerBig0, idEq);

    std::shared_ptr<Predicate> containerLess0 = calculator->createLesser(containerVar, calculator->getNumber(0));
    std::shared_ptr<Predicate> idNeq = calculator->createEquality(containerId, calculator->getNumber(liquidId));
    containerLess0 = calculator->andPredicates(containerLess0, idNeq);

    mainPred = calculator->orPredicates(mainPred, containerBig0);
    mainPred = calculator->orPredicates(mainPred, containerLess0);

    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateCloseContainer(int idCloseContainer) throw(std::invalid_argument) {
    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(idCloseContainer);
    MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(idCloseContainer);

    if (!leaving->empty() && !arriving->empty()) {
        std::shared_ptr<Predicate> combinationsPred = makeNodeTubesCombinations(idCloseContainer, true, *arriving.get(), *leaving.get());

        std::shared_ptr<Predicate> zeroState = NULL;
        if (leaving->size() + arriving->size() > 2) {
            std::shared_ptr<Predicate> arrivingZero = allTubesSameEquality(*arriving.get(), Equality::equal, calculator->getNumber(0));
            std::shared_ptr<Predicate> leavingZero = allTubesSameEquality(*leaving.get(), Equality::equal, calculator->getNumber(0));
            std::shared_ptr<Predicate> containerEq = calculator->createEquality(VariableNominator::getContainerVarName(idCloseContainer), 0);

            zeroState = calculator->andPredicates(arrivingZero, leavingZero);
            zeroState = calculator->andPredicates(zeroState, containerEq);
        }

        combinationsPred = calculator->orPredicates(zeroState, combinationsPred);
        return combinationsPred;
    } else {
        throw(std::invalid_argument("ShortStatePredicateGenerator::generatePredicateCloseContainer." + std::to_string(idCloseContainer) + " close container must has tubes leaving and arriving"));
    }
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

    std::shared_ptr<Variable> tubeVar = calculator->getVariable(VariableNominator::getTubeVarName(rateEdge->getIdSource(), rateEdge->getIdTarget()));
    std::shared_ptr<Predicate> rate = calculator->createEquality(VariableNominator::getPumpRateVarName(pumpId),
                                                                 calculator->calculateRate(tubeVar));

    std::shared_ptr<Predicate> arrivingSign = allTubesSameEquality(tubesArriving, op, calculator->getNumber(0));
    std::shared_ptr<Predicate> leavingSign = allTubesSameEquality(tubesLeaving, op, calculator->getNumber(0));

    std::shared_ptr<ArithmeticOperable> arrivingSum = NULL;
    if ((tubesArriving.size() == 1) ) {
        MachineGraph::GraphType::EdgeTypePtr aTube = tubesArriving.back();
        arrivingSum = calculator->getVariable(VariableNominator::getTubeVarName(aTube->getIdSource(), aTube->getIdTarget()));
    } else {
        arrivingSum = addAllTubesProperty(tubesArriving, *calculator.get(), &CommomRulesOperations::calculateId);
        std::shared_ptr<Variable> arrivingRate = calculator->getVariable(VariableNominator::getTubeVarName(rateEdge->getIdSource(), rateEdge->getIdTarget()));
        arrivingSum = calculator->addOperables(arrivingSum, calculator->calculateRate(arrivingRate));

        if (op == Equality::lesser) {
            arrivingSum = calculator->changeSignAbs(arrivingSum);
        }
    }

    std::shared_ptr<Predicate> leavingTubesEq = NULL;
    for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubesLeaving) {
        std::shared_ptr<Predicate> actualEq = calculator->createEquality(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()),
                                                                         arrivingSum);
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

    while(arrivingZeroIt.calculateNext()) {
        Label dummy;
        Label zero;
        zero.setLabel(Label::zero);

        LabelCombinationsIterator::LabelSet arrivinglabelSet = arrivingZeroIt.getLabelSet();
        if ((arrivinglabelSet.find(dummy) != arrivinglabelSet.end()))
        {
            LabelCombinationsIterator leavingZeroIt(labelMapLeaving, types);
            while(leavingZeroIt.calculateNext()) {
                LabelCombinationsIterator::LabelSet leavingLabelSet = leavingZeroIt.getLabelSet();
                if ((leavingLabelSet.find(dummy) != leavingLabelSet.end()))
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

    MachineGraph::GraphType::EdgeVector arrivingNonZero;
    MachineGraph::GraphType::EdgeVector  arrivingZero;
    separateLabelMap(arrivingMap, Label::zero, arrivingZero, arrivingNonZero);

    MachineGraph::GraphType::EdgeVector  leavingNonZero;
    MachineGraph::GraphType::EdgeVector  leavingZero;
    separateLabelMap(leavingMap, Label::zero, leavingZero, leavingNonZero);

    std::shared_ptr<Predicate> tubesZeroA = allTubesSameEquality(arrivingZero, Equality::equal, calculator->getNumber(0));
    std::shared_ptr<Predicate> tubesZeroL = allTubesSameEquality(leavingZero, Equality::equal, calculator->getNumber(0));

    mainPred = calculator->andPredicates(tubesZeroA, tubesZeroL);

    std::shared_ptr<Predicate> tubesRateA = samePropertyTube(arrivingNonZero, *calculator.get(), &CommomRulesOperations::calculateRate);
    std::shared_ptr<Predicate> tubesRateL = samePropertyTube(leavingNonZero, *calculator.get(), &CommomRulesOperations::calculateRate);

    if (!arrivingNonZero.empty() && !leavingNonZero.empty()) {
        MachineGraph::GraphType::EdgeTypePtr tubeA = arrivingNonZero.back();
        MachineGraph::GraphType::EdgeTypePtr tubeL = *leavingNonZero.begin();

        std::shared_ptr<Variable> tubeAVar = calculator->getVariable(VariableNominator::getTubeVarName(tubeA->getIdSource(), tubeA->getIdTarget()));
        std::shared_ptr<Variable> tubeLVar = calculator->getVariable(VariableNominator::getTubeVarName(tubeL->getIdSource(), tubeL->getIdTarget()));

        std::shared_ptr<Predicate> arrivingLeavingRateEq = calculator->createEquality(calculator->calculateRate(tubeAVar), calculator->calculateRate(tubeLVar));
        tubesRateA = calculator->andPredicates(tubesRateA, arrivingLeavingRateEq);
    }

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



bool ShortStatePredicateGenerator::hasLabel(Label::LabelType type,
                                            const LabelTypeTubeMap & labelMap,
                                            const MachineGraph::GraphType::EdgeVector & tubes)
    throw(std::invalid_argument)
{
    bool hasLabel = false;

    auto it = tubes.begin();
    while(!hasLabel && (it != tubes.end())) {
        MachineGraph::GraphType::EdgeTypePtr tube = *it;
        std::tuple<int,int> tubeID = std::make_tuple(tube->getIdSource(), tube->getIdTarget());

        auto finded = labelMap.find(tubeID);
        if (finded != labelMap.end()) {
            Label label = finded->second;
            hasLabel = label.hasLabelMask(type);
        } else {
            throw(std::invalid_argument("ShortStatePredicateGenerator::hasLabel. Unknow tube " + tube->toText()));
        }
        ++it;
    }
    return hasLabel;
}

bool ShortStatePredicateGenerator::hasLabel(Label::LabelType type, const LabelTypeTubeMap & labelMap) {
    bool hasLabel = false;

    auto it = labelMap.begin();
    while (!hasLabel && (it != labelMap.end())) {
        Label actualLabel = it->second;
        hasLabel = actualLabel.hasLabelMask(type);
        ++it;
    }
    return hasLabel;
}

ShortStatePredicateGenerator::LabelTypeTubeMap ShortStatePredicateGenerator::makeLabelMap(
        const MachineGraph::GraphType::EdgeVector & tubes,
        const Label & initValue)
{
    LabelTypeTubeMap labelMap;
    for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubes) {
        std::tuple<int,int> tubeId = std::make_tuple(tube->getIdSource(), tube->getIdTarget());
        labelMap.insert(std::make_pair(tubeId, initValue));
    }
    return labelMap;
}

void ShortStatePredicateGenerator::separateLabelMap(const LabelTypeTubeMap & map,
                                                    Label::LabelType type,
                                                    MachineGraph::GraphType::EdgeVector & tubesType,
                                                    MachineGraph::GraphType::EdgeVector & tubesNonType)
{
    for(auto pair: map) {
        const std::tuple<int,int> & tubeId = pair.first;
        Label label = pair.second;

        if (label.hasLabelMask(type)) {
            MachineGraph::GraphType::EdgeTypePtr tube = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));
            tubesType.push_back(tube);
        } else {
            MachineGraph::GraphType::EdgeTypePtr tube = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));
            tubesNonType.push_back(tube);
        }
    }
}

void ShortStatePredicateGenerator::separateLabelMap(const LabelTypeTubeMap & map,
                                                    const Label & label,
                                                    MachineGraph::GraphType::EdgeVector & tubesType,
                                                    MachineGraph::GraphType::EdgeVector & tubesNonType)
{
    for(auto pair: map) {
        const std::tuple<int,int> & tubeId = pair.first;
        Label actualLabel = pair.second;

        if (label == actualLabel) {
            MachineGraph::GraphType::EdgeTypePtr tube = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));
            tubesType.push_back(tube);
        } else {
            MachineGraph::GraphType::EdgeTypePtr tube = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));
            tubesNonType.push_back(tube);
        }
    }
}

void ShortStatePredicateGenerator::subtractTubesWithoutLabel(
        const LabelTypeTubeMap & map,
        Label::LabelType type,
        MachineGraph::GraphType::EdgeVector & tubesToSubstract,
        MachineGraph::GraphType::EdgeVector & tubesToAddSubstracted)
    throw(std::invalid_argument)
{
    for(auto it = tubesToSubstract.begin(); it != tubesToSubstract.end();) {
        MachineGraph::GraphType::EdgeTypePtr tubePtr = *it;
        std::tuple<int,int> tubeId = std::make_tuple(tubePtr->getIdSource(), tubePtr->getIdTarget());

        auto finded = map.find(tubeId);
        if (finded != map.end()) {

            Label actualLabel = finded->second;
            if (!actualLabel.hasLabelMask(type))  {
                it = tubesToSubstract.erase(it);
                tubesToAddSubstracted.push_back(tubePtr);
            } else {
                ++it;
            }

        } else {
            throw(std::invalid_argument("ShortStatePredicateGenerator::subtractTubesWithoutLabel(): Tube: " +
                                        std::to_string(std::get<0>(tubeId)) + "->" + std::to_string(std::get<1>(tubeId)) + " is not present on the map."));
        }
    }
}

void ShortStatePredicateGenerator::separateDirMap(
        const TubeDirMap & dirMap,
        MachineGraph::GraphType::EdgeVector & tubesRegular,
        MachineGraph::GraphType::EdgeVector & tubesInverted)
{
    for(const auto & it: dirMap) {
        const std::tuple<int,int> & tubeId = it.first;
        MachineGraph::GraphType::EdgeTypePtr tubePtr = graph->getTube(std::get<0>(tubeId), std::get<1>(tubeId));

        TubeEdge::TubeDirection dir = it.second;
        if (dir == TubeEdge::inverted) {
            tubesInverted.push_back(tubePtr);
        } else if (dir == TubeEdge::regular) {
            tubesRegular.push_back(tubePtr);
        }
    }
}

ShortStatePredicateGenerator::TubeDirMap ShortStatePredicateGenerator::makeTubeDirMap(const MachineGraph::GraphType::EdgeVector & tubes) {
    TubeDirMap dirMap;
    for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubes) {
        std::tuple<int,int> tubeId = std::make_tuple(tube->getIdSource(), tube->getIdTarget());
        dirMap.insert(std::make_pair(tubeId, TubeEdge::unknow));
    }
    return dirMap;
}

bool ShortStatePredicateGenerator::canApplyDirArriving(Label::LabelType type, const TubeDirMap & arrivingMap, const TubeDirMap & leavingMap) {
    bool canAply = true;
    if (type == Label::bigger) {
        for(auto it = arrivingMap.begin(); canAply && it != arrivingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::inverted));
        }

        for(auto it = leavingMap.begin(); canAply && it != leavingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::regular));
        }
    } else if (type == Label::smaller) {
        for(auto it = arrivingMap.begin(); canAply && it != arrivingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::regular));
        }

        for(auto it = leavingMap.begin(); canAply && it != leavingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::inverted));
        }
    }
    return canAply;
}

bool ShortStatePredicateGenerator::canApplyDirLeaving(Label::LabelType type, const TubeDirMap & arrivingMap, const TubeDirMap & leavingMap) {
    bool canAply = true;
    if (type == Label::bigger) {
        for(auto it = arrivingMap.begin(); canAply && it != arrivingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::regular));
        }

        for(auto it = leavingMap.begin(); canAply && it != leavingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::inverted));
        }
    } else if (type == Label::smaller) {
        for(auto it = arrivingMap.begin(); canAply && it != arrivingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::inverted));
        }

        for(auto it = leavingMap.begin(); canAply && it != leavingMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            canAply = ((dir == TubeEdge::unknow) || (dir == TubeEdge::regular));
        }
    }
    return canAply;
}

void ShortStatePredicateGenerator::applyDirArriving(Label::LabelType type, TubeDirMap & arrivingMap, TubeDirMap & leavingMap) {
    if (type == Label::bigger) {
        for(auto it = arrivingMap.begin(); it != arrivingMap.end(); ++it) {
            it->second = TubeEdge::inverted;
        }
        for(auto it = leavingMap.begin(); it != leavingMap.end(); ++it) {
            it->second = TubeEdge::regular;
        }
    } else if (type == Label::smaller) {
        for(auto it = arrivingMap.begin(); it != arrivingMap.end(); ++it) {
            it->second = TubeEdge::regular;
        }
        for(auto it = leavingMap.begin(); it != leavingMap.end(); ++it) {
            it->second = TubeEdge::inverted;
        }
    }
}

void ShortStatePredicateGenerator::applyDirLeaving(Label::LabelType type, TubeDirMap & arrivingMap, TubeDirMap & leavingMap) {
    if (type == Label::bigger) {
        for(auto it = arrivingMap.begin(); it != arrivingMap.end(); ++it) {
            it->second = TubeEdge::regular;
        }
        for(auto it = leavingMap.begin(); it != leavingMap.end(); ++it) {
            it->second = TubeEdge::inverted;
        }
    } else if (type == Label::smaller) {
        for(auto it = arrivingMap.begin(); it != arrivingMap.end(); ++it) {
            it->second = TubeEdge::inverted;
        }
        for(auto it = leavingMap.begin(); it != leavingMap.end(); ++it) {
            it->second = TubeEdge::regular;
        }
    }
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateValve(int valveId) {
    Label zeroLabel;
    zeroLabel.setLabel(Label::zero);

    MachineGraph::GraphType::EdgeVectorPtr leaving = graph->getLeavingTubes(valveId);
    MachineGraph::GraphType::EdgeVectorPtr arriving = graph->getArrivingTubes(valveId);

    std::shared_ptr<ValveNode> valvePtr = graph->getValve(valveId);

    std::shared_ptr<Predicate> mainPred = NULL;
    for(int possition: valvePtr->getValvePossibleValues()) {
        LabelTypeTubeMap leavingMap = makeLabelMap(*leaving.get(), zeroLabel);
        LabelTypeTubeMap arrivingMap = makeLabelMap(*arriving.get(), zeroLabel);

        std::shared_ptr<Predicate> tubesConnectedpredicates =
                generatePredicateValve_processConnectedPins(valveId, possition, valvePtr, arrivingMap, leavingMap);

        if (tubesConnectedpredicates != NULL) {
            std::shared_ptr<Predicate> allowTubesZero = allTubesSameEquality(*arriving, Equality::equal, calculator->getNumber(0));
            allowTubesZero = calculator->andPredicates(allowTubesZero, allTubesSameEquality(*leaving, Equality::equal, calculator->getNumber(0)));

            tubesConnectedpredicates = calculator->orPredicates(allowTubesZero, tubesConnectedpredicates);
        }

        MachineGraph::GraphType::EdgeVector aTubesZero;
        MachineGraph::GraphType::EdgeVector lTubesZero;

        MachineGraph::GraphType::EdgeVector empty;
        separateLabelMap(arrivingMap, Label::zero, aTubesZero, empty);
        separateLabelMap(leavingMap, Label::zero, lTubesZero, empty);

        std::shared_ptr<Predicate> valvePossitionEq = calculator->createEquality(VariableNominator::getValveVarName(valveId), possition);

        std::shared_ptr<Predicate> tubesZeroEq = allTubesSameEquality(aTubesZero, Equality::equal, calculator->getNumber(0));
        tubesZeroEq = calculator->andPredicates(tubesZeroEq, allTubesSameEquality(lTubesZero, Equality::equal, calculator->getNumber(0)));

        std::shared_ptr<Predicate> positionPred = calculator->andPredicates(valvePossitionEq, tubesZeroEq);
        positionPred = calculator->andPredicates(positionPred, tubesConnectedpredicates);

        mainPred = calculator->orPredicates(mainPred, positionPred);
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::generatePredicateValve_processConnectedPins(
            int valveId,
            int possition,
            std::shared_ptr<ValveNode> valvePtr,
            LabelTypeTubeMap & arrivingMap,
            LabelTypeTubeMap & leavingMap)
    throw(std::invalid_argument)
{
    std::shared_ptr<Predicate> pinSetPredicates = NULL;
    std::shared_ptr<Predicate> pinSetNotZero = NULL;
    for(const std::unordered_set<int> & connectedPins : valvePtr->getConnectedPins(possition)) {
        MachineGraph::GraphType::EdgeVector connectedArriving;
        MachineGraph::GraphType::EdgeVector connectedLeaving;

        for (int pin : connectedPins) {
            const std::tuple<int, int> & tubeId = valvePtr->getTubeIdConnectedToPin(pin);

            if (std::get<0>(tubeId) == valveId) { //is leaving
                connectedLeaving.push_back(graph->getTubeConnectedToPin(valveId, pin));

                auto finded = leavingMap.find(tubeId);
                if (finded != leavingMap.end()) {
                    finded->second = dummy;
                } else {
                    throw(std::invalid_argument("ShortStatePredicateGenerator::generatePredicateValve_processConnectedPins. Tube:" +
                                                std::to_string(std::get<0>(tubeId)) + "->" + std::to_string(std::get<1>(tubeId)) +
                                                " not present in leavingMap"));
                }
            } else if (std::get<1>(tubeId) == valveId) { //is arriving
                connectedArriving.push_back(graph->getTubeConnectedToPin(valveId, pin));

                auto finded = arrivingMap.find(tubeId);
                if (finded != arrivingMap.end()) {
                    finded->second = dummy;
                } else {
                    throw(std::invalid_argument("ShortStatePredicateGenerator::generatePredicateValve_processConnectedPins. Tube:" +
                                                std::to_string(std::get<0>(tubeId)) + "->" + std::to_string(std::get<1>(tubeId)) +
                                                " not present in arrivingMap"));
                }
            }
        }

        std::shared_ptr<Predicate> actualpinsSetPredicate = makeNodeTubesCombinations(valveId, false, connectedArriving, connectedLeaving);
        pinSetPredicates = calculator->andPredicates(pinSetPredicates, actualpinsSetPredicate);

        if((connectedArriving.size() == 1) &&
           (connectedLeaving.size() == 1))
        {
            MachineGraph::GraphType::EdgeTypePtr tubeA = connectedArriving.back();
            MachineGraph::GraphType::EdgeTypePtr tubeL = connectedLeaving.back();

            MachineGraph::GraphType::EdgeVector tubesNotzero = {tubeA, tubeL};
            pinSetNotZero = calculator->orPredicates(pinSetNotZero, allTubesSameEquality(tubesNotzero, Equality::not_equal, calculator->getNumber(0)));
        }
    }

    return calculator->andPredicates(pinSetNotZero, pinSetPredicates);
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations(
        int nodeId,
        bool makeContainerEq,
        const MachineGraph::GraphType::EdgeVector & tubesArriving,
        const MachineGraph::GraphType::EdgeVector & tubesLeaving)
    throw(std::invalid_argument)
{
    std::shared_ptr<Predicate> mainPred = NULL;
    if (tubesArriving.size() + tubesLeaving.size() == 2) {
        MachineGraph::GraphType::EdgeTypePtr aTube;
        MachineGraph::GraphType::EdgeTypePtr lTube;

        if (tubesArriving.empty()) { //both leaving
            aTube = tubesLeaving[0];
            lTube = tubesLeaving[1];
        } else if (tubesLeaving.empty()) { //both arriving
            aTube = tubesArriving[0];
            lTube = tubesArriving[1];
        } else { //arriving and leaving
            aTube = tubesArriving.back();
            lTube = tubesLeaving.back();
        }

        mainPred = calculator->createEquality(VariableNominator::getTubeVarName(lTube->getIdSource(), lTube->getIdTarget()),
                                              VariableNominator::getTubeVarName(aTube->getIdSource(), aTube->getIdTarget()));
        if (makeContainerEq) {
            std::shared_ptr<Predicate> containerEq = calculator->createEquality(VariableNominator::getContainerVarName(nodeId),
                                                                                VariableNominator::getTubeVarName(aTube->getIdSource(), aTube->getIdTarget()));
            mainPred = calculator->andPredicates(mainPred, containerEq);
        }
    } else {
        LabelTypeTubeMap labelMapArriving;
        labelTubeSet(true, tubesArriving, labelMapArriving);

        LabelTypeTubeMap labelMapLeaving;
        labelTubeSet(false, tubesLeaving, labelMapLeaving);

        MachineGraph::GraphType::EdgeVector arrivingDummy;
        MachineGraph::GraphType::EdgeVector arrivingNonDummy;
        separateLabelMap(labelMapArriving, dummy, arrivingDummy, arrivingNonDummy);

        MachineGraph::GraphType::EdgeVector leavingDummy;
        MachineGraph::GraphType::EdgeVector leavingNonDummy;
        separateLabelMap(labelMapLeaving, dummy, leavingDummy, leavingNonDummy);

        if(!leavingNonDummy.empty() && arrivingNonDummy.empty()) {
            mainPred = makeNodeTubesCombinations_makeIteratorsLeaving(nodeId,
                                                                      makeContainerEq,
                                                                      arrivingDummy,
                                                                      leavingDummy,
                                                                      getSubMap(labelMapLeaving, leavingNonDummy));
        } else if (leavingNonDummy.empty() && !arrivingNonDummy.empty()) {
            mainPred = makeNodeTubesCombinations_makeIteratorsArriving(nodeId,
                                                                       makeContainerEq,
                                                                       arrivingDummy,
                                                                       leavingDummy,
                                                                       getSubMap(labelMapArriving, arrivingNonDummy));
        } else if(!leavingNonDummy.empty() && !arrivingNonDummy.empty()) {
            mainPred = makeNodeTubesCombinations_makeIteratorsBoth(nodeId,
                                                                   makeContainerEq,
                                                                   arrivingDummy,
                                                                   getSubMap(labelMapArriving, arrivingNonDummy),
                                                                   leavingDummy,
                                                                   getSubMap(labelMapLeaving, leavingNonDummy));
        } else {
            throw(std::invalid_argument("ShortStatePredicateGenerator::makeNodeTubesCombinations. Node " + std::to_string(nodeId)  +
                                        "Is not connected to any pump or valve"));
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_makeIteratorsLeaving(
        int nodeId,
        bool makeContainerEq,
        const MachineGraph::GraphType::EdgeVector & arrivingDummy,
        const MachineGraph::GraphType::EdgeVector & leavingDummy,
        const LabelTypeTubeMap & leavingMap)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::vector<Label::LabelType> zero = {Label::zero};
    LabelCombinationsIterator leavingIt(leavingMap, zero);

    while(leavingIt.calculateNext()) {
        const LabelCombinationsIterator::LabelSet & leavItSet = leavingIt.getLabelSet();

        if ((leavItSet.find(dummy) != leavItSet.end())) {
            //if a dummy label is set at least one tube is not zero
            MachineGraph::GraphType::EdgeVector leavingZero;
            MachineGraph::GraphType::EdgeVector leavingNonZero;
            separateLabelMap(leavingIt.getTubesLabelsMap(), Label::zero, leavingZero, leavingNonZero);

            MachineGraph::GraphType::EdgeVector tempLeavingDummy(leavingDummy);
            subtractTubesWithoutLabel(leavingMap, Label::bigger, leavingNonZero, tempLeavingDummy);

            if (!leavingNonZero.empty() &&
                (leavingNonZero.size() + tempLeavingDummy.size() >= 1))
            {
                LabelTypeTubeMap labelsNonZeroLeaving = getSubMap(leavingMap, leavingNonZero);
                std::shared_ptr<Predicate> combinationPredicates =
                        makeNodeTubesCombinations_combinationNonZerosLeaving(nodeId,
                                                                             makeContainerEq,
                                                                             labelsNonZeroLeaving,
                                                                             arrivingDummy,
                                                                             tempLeavingDummy);
                if (combinationPredicates != NULL) {
                    std::shared_ptr<Predicate> tubesZero = allTubesSameEquality(leavingZero, Equality::equal, calculator->getNumber(0));
                    combinationPredicates = calculator->andPredicates(tubesZero, combinationPredicates);

                    mainPred = calculator->orPredicates(mainPred, combinationPredicates);
                }
            }
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_makeIteratorsBoth(
        int nodeId,
        bool makeContainerEq,
        const MachineGraph::GraphType::EdgeVector & arrivingDummy,
        const LabelTypeTubeMap & arrivingMap,
        const MachineGraph::GraphType::EdgeVector & leavingDummy,
        const LabelTypeTubeMap & leavingMap)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::vector<Label::LabelType> zero = {Label::zero};
    LabelCombinationsIterator arrivingIt(arrivingMap, zero);

    while(arrivingIt.calculateNext()) {
        MachineGraph::GraphType::EdgeVector arrivingZero;
        MachineGraph::GraphType::EdgeVector arrivingNonZero;
        separateLabelMap(arrivingIt.getTubesLabelsMap(), Label::zero, arrivingZero, arrivingNonZero);

        MachineGraph::GraphType::EdgeVector tempArrivingDummy(arrivingDummy);
        subtractTubesWithoutLabel(arrivingMap, Label::bigger, arrivingNonZero, tempArrivingDummy);

        LabelCombinationsIterator leavingIt(leavingMap, zero);
        while(leavingIt.calculateNext()) {
            const LabelCombinationsIterator::LabelSet & arrivItSet = arrivingIt.getLabelSet();
            const LabelCombinationsIterator::LabelSet & leavItSet = leavingIt.getLabelSet();

            if ((arrivItSet.find(dummy) != arrivItSet.end()) ||
                    (leavItSet.find(dummy) != leavItSet.end()))
            {
                //if a dummy label is set at least one tube is not zero
                MachineGraph::GraphType::EdgeVector leavingZero;
                MachineGraph::GraphType::EdgeVector leavingNonZero;
                separateLabelMap(leavingIt.getTubesLabelsMap(), Label::zero, leavingZero, leavingNonZero);

                MachineGraph::GraphType::EdgeVector tempLeavingDummy(leavingDummy);
                subtractTubesWithoutLabel(leavingMap, Label::bigger, leavingNonZero, tempLeavingDummy);

                if((leavingNonZero.size() + arrivingNonZero.size() + tempLeavingDummy.size() + tempArrivingDummy.size()) > 1) {

                    std::shared_ptr<Predicate> combinationPredicates = NULL;
                    if (leavingNonZero.empty() && !arrivingNonZero.empty()) { //arriving
                        LabelTypeTubeMap labelsNonZeroArriving = getSubMap(arrivingMap, arrivingNonZero);
                        combinationPredicates = makeNodeTubesCombinations_combinationNonZerosArriving(nodeId,
                                                                                                      makeContainerEq,
                                                                                                      labelsNonZeroArriving,
                                                                                                      tempArrivingDummy,
                                                                                                      tempLeavingDummy);
                    } else if (!leavingNonZero.empty() && arrivingNonZero.empty()) {//leaving
                        LabelTypeTubeMap labelsNonZeroLeaving = getSubMap(leavingMap, leavingNonZero);
                        combinationPredicates = makeNodeTubesCombinations_combinationNonZerosLeaving(nodeId,
                                                                                                     makeContainerEq,
                                                                                                     labelsNonZeroLeaving,
                                                                                                     tempArrivingDummy,
                                                                                                     tempLeavingDummy);
                    } else if (!leavingNonZero.empty() && !arrivingNonZero.empty()) { // both
                        LabelTypeTubeMap labelsNonZeroArriving = getSubMap(arrivingMap, arrivingNonZero);
                        LabelTypeTubeMap labelsNonZeroLeaving = getSubMap(leavingMap, leavingNonZero);
                        combinationPredicates =
                                makeNodeTubesCombinations_combinationNonZerosBoth(nodeId,
                                                                                  makeContainerEq,
                                                                                  labelsNonZeroArriving,
                                                                                  labelsNonZeroLeaving,
                                                                                  tempArrivingDummy,
                                                                                  tempLeavingDummy);
                    }

                    if (combinationPredicates != NULL) {
                        std::shared_ptr<Predicate> tubesZero = allTubesSameEquality(arrivingZero, Equality::equal, calculator->getNumber(0));
                        tubesZero = calculator->andPredicates(tubesZero, allTubesSameEquality(leavingZero, Equality::equal, calculator->getNumber(0)));
                        combinationPredicates = calculator->andPredicates(tubesZero, combinationPredicates);

                        mainPred = calculator->orPredicates(mainPred, combinationPredicates);
                    }
                }
            }
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_makeIteratorsArriving(
        int nodeId,
        bool makeContainerEq,
        const MachineGraph::GraphType::EdgeVector & arrivingDummy,
        const MachineGraph::GraphType::EdgeVector & leavingDummy,
        const LabelTypeTubeMap & arrivingMap)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::vector<Label::LabelType> zero = {Label::zero};
    LabelCombinationsIterator arrivingIt(arrivingMap, zero);

    while(arrivingIt.calculateNext()) {
        const LabelCombinationsIterator::LabelSet & arrivItSet = arrivingIt.getLabelSet();

        if (arrivItSet.find(dummy) != arrivItSet.end()) {
            //if a dummy label is set at least one tube is not zero
            MachineGraph::GraphType::EdgeVector arrivingZero;
            MachineGraph::GraphType::EdgeVector arrivingNonZero;
            separateLabelMap(arrivingIt.getTubesLabelsMap(), Label::zero, arrivingZero, arrivingNonZero);

            MachineGraph::GraphType::EdgeVector tempArrivingDummy(arrivingDummy);
            subtractTubesWithoutLabel(arrivingMap, Label::bigger, arrivingNonZero, tempArrivingDummy);


            if (!arrivingNonZero.empty() &&
               ((arrivingNonZero.size() + tempArrivingDummy.size()) > 1))
            {
                LabelTypeTubeMap labelsNonZeroArriving = getSubMap(arrivingMap, arrivingNonZero);
                std::shared_ptr<Predicate> combinationPredicates =
                        makeNodeTubesCombinations_combinationNonZerosArriving(nodeId,
                                                                              makeContainerEq,
                                                                              labelsNonZeroArriving,
                                                                              tempArrivingDummy,
                                                                              leavingDummy);
                if (combinationPredicates != NULL) {
                    std::shared_ptr<Predicate> tubesZero = allTubesSameEquality(arrivingZero, Equality::equal, calculator->getNumber(0));
                    combinationPredicates = calculator->andPredicates(tubesZero, combinationPredicates);

                    mainPred = calculator->orPredicates(mainPred, combinationPredicates);
                }
            }
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_combinationNonZerosBoth(
        int nodeId,
        bool makeContainerEq,
        const LabelTypeTubeMap & labelsArriving,
        const LabelTypeTubeMap & labelsLeaving,
        const MachineGraph::GraphType::EdgeVector & dummyArriving,
        const MachineGraph::GraphType::EdgeVector & dummyLeaving)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::vector<Label::LabelType> types = {Label::bigger, Label::smaller};
    LabelCombinationsIterator arrivingIt(labelsArriving, types, false);

    while(arrivingIt.calculateNext()) {
        MachineGraph::GraphType::EdgeVector aBigger;
        MachineGraph::GraphType::EdgeVector aSmaller;
        separateLabelMap(arrivingIt.getTubesLabelsMap(), Label::bigger, aBigger, aSmaller);

        LabelCombinationsIterator leavingIt(labelsLeaving, types, false);
        while(leavingIt.calculateNext()) {
            TubeDirMap aDirMap = makeTubeDirMap(dummyArriving);
            TubeDirMap lDirMap = makeTubeDirMap(dummyLeaving);

            bool canApply = true;
            if (!aBigger.empty()) {
                canApply = canApplyDirArriving(Label::bigger, aDirMap, lDirMap);
                if (canApply) {
                    applyDirArriving(Label::bigger, aDirMap, lDirMap);
                }
            }

            if (canApply && !aSmaller.empty()) {
                canApply = canApplyDirArriving(Label::smaller, aDirMap, lDirMap);
                if (canApply) {
                    applyDirArriving(Label::smaller, aDirMap, lDirMap);
                }
            }

            if(canApply) {
                MachineGraph::GraphType::EdgeVector lBigger;
                MachineGraph::GraphType::EdgeVector lSmaller;
                separateLabelMap(leavingIt.getTubesLabelsMap(), Label::bigger, lBigger, lSmaller);

                if (!lBigger.empty()) {
                    canApply = canApplyDirLeaving(Label::bigger, aDirMap, lDirMap);
                    if (canApply) {
                        applyDirLeaving(Label::bigger, aDirMap, lDirMap);
                    }
                }

                if (canApply && !lSmaller.empty()) {
                    canApply = canApplyDirLeaving(Label::smaller, aDirMap, lDirMap);
                    if (canApply) {
                        applyDirLeaving(Label::smaller, aDirMap, lDirMap);
                    }
                }

                if(canApply && checkBothDirection(aDirMap, lDirMap, aBigger, aSmaller, lBigger, lSmaller)) {
                    MachineGraph::GraphType::EdgeVector aDummyBigger;
                    MachineGraph::GraphType::EdgeVector aDummySmaller;
                    MachineGraph::GraphType::EdgeVector lDummyBigger;
                    MachineGraph::GraphType::EdgeVector lDummySmaller;

                    separateDirMap(aDirMap, aDummyBigger, aDummySmaller);
                    separateDirMap(lDirMap, lDummyBigger, lDummySmaller);

                    std::shared_ptr<Predicate> combinationPredicate =
                            makeNodeTubesCombinations_processCombination(nodeId, makeContainerEq, aDummyBigger, aDummySmaller, lDummyBigger,
                                                                         lDummySmaller, aBigger, aSmaller, lBigger, lSmaller);

                    mainPred = calculator->orPredicates(mainPred, combinationPredicate);
                }
            }
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_combinationNonZerosLeaving(
        int nodeId,
        bool makeContainerEq,
        const LabelTypeTubeMap & labelsLeaving,
        const MachineGraph::GraphType::EdgeVector & dummyArriving,
        const MachineGraph::GraphType::EdgeVector & dummyLeaving)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::vector<Label::LabelType> types = {Label::bigger, Label::smaller};
    LabelCombinationsIterator leavingIt(labelsLeaving, types, false);

    while(leavingIt.calculateNext()) {
        TubeDirMap aDirMap = makeTubeDirMap(dummyArriving);
        TubeDirMap lDirMap = makeTubeDirMap(dummyLeaving);

        bool canApply = true;
        MachineGraph::GraphType::EdgeVector lBigger;
        MachineGraph::GraphType::EdgeVector lSmaller;
        separateLabelMap(leavingIt.getTubesLabelsMap(), Label::bigger, lBigger, lSmaller);

        if (!lBigger.empty()) {
            canApply = canApplyDirLeaving(Label::bigger, aDirMap, lDirMap);
            if (canApply) {
                applyDirLeaving(Label::bigger, aDirMap, lDirMap);
            }
        }

        if (canApply && !lSmaller.empty()) {
            canApply = canApplyDirLeaving(Label::smaller, aDirMap, lDirMap);
            if (canApply) {
                applyDirLeaving(Label::smaller, aDirMap, lDirMap);
            }
        }

        canApply = canApply & checkBothDirection(aDirMap,
                                                 lDirMap,
                                                 MachineGraph::GraphType::EdgeVector(),
                                                 MachineGraph::GraphType::EdgeVector(),
                                                 lBigger,
                                                 lSmaller);
        if(canApply) {
            MachineGraph::GraphType::EdgeVector aDummyBigger;
            MachineGraph::GraphType::EdgeVector aDummySmaller;
            MachineGraph::GraphType::EdgeVector lDummyBigger;
            MachineGraph::GraphType::EdgeVector lDummySmaller;

            separateDirMap(aDirMap, aDummyBigger, aDummySmaller);
            separateDirMap(lDirMap, lDummyBigger, lDummySmaller);

            std::shared_ptr<Predicate> combinationPredicate =
                    makeNodeTubesCombinations_processCombination(nodeId, makeContainerEq, aDummyBigger, aDummySmaller,
                                                                 lDummyBigger, lDummySmaller, MachineGraph::GraphType::EdgeVector(),
                                                                 MachineGraph::GraphType::EdgeVector(), lBigger, lSmaller);

            mainPred = calculator->orPredicates(mainPred, combinationPredicate);
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_combinationNonZerosArriving(
        int nodeId,
        bool makeContainerEq,
        const LabelTypeTubeMap & labelsArriving,
        const MachineGraph::GraphType::EdgeVector & dummyArriving,
        const MachineGraph::GraphType::EdgeVector & dummyLeaving)
{
    std::shared_ptr<Predicate> mainPred = NULL;

    std::vector<Label::LabelType> types = {Label::bigger, Label::smaller};
    LabelCombinationsIterator arrivingIt(labelsArriving, types, false);

    while(arrivingIt.calculateNext()) {
        TubeDirMap aDirMap = makeTubeDirMap(dummyArriving);
        TubeDirMap lDirMap = makeTubeDirMap(dummyLeaving);

        MachineGraph::GraphType::EdgeVector aBigger;
        MachineGraph::GraphType::EdgeVector aSmaller;
        separateLabelMap(arrivingIt.getTubesLabelsMap(), Label::bigger, aBigger, aSmaller);

        bool canApply = true;
        if (!aBigger.empty()) {
            canApply = canApplyDirArriving(Label::bigger, aDirMap, lDirMap);
            if (canApply) {
                applyDirArriving(Label::bigger, aDirMap, lDirMap);
            }
        }
        if (canApply && !aSmaller.empty()) {
            canApply = canApplyDirArriving(Label::smaller, aDirMap, lDirMap);
            if (canApply) {
                applyDirArriving(Label::smaller, aDirMap, lDirMap);
            }
        }

        canApply = canApply & checkBothDirection(aDirMap,
                                                 lDirMap,
                                                 aBigger,
                                                 aSmaller,
                                                 MachineGraph::GraphType::EdgeVector(),
                                                 MachineGraph::GraphType::EdgeVector());
        if(canApply) {
            MachineGraph::GraphType::EdgeVector aDummyBigger;
            MachineGraph::GraphType::EdgeVector aDummySmaller;
            MachineGraph::GraphType::EdgeVector lDummyBigger;
            MachineGraph::GraphType::EdgeVector lDummySmaller;

            separateDirMap(aDirMap, aDummyBigger, aDummySmaller);
            separateDirMap(lDirMap, lDummyBigger, lDummySmaller);

            std::shared_ptr<Predicate> combinationPredicate =
                    makeNodeTubesCombinations_processCombination(nodeId, makeContainerEq, aDummyBigger, aDummySmaller,
                                                                 lDummyBigger, lDummySmaller, aBigger, aSmaller,
                                                                 MachineGraph::GraphType::EdgeVector(), MachineGraph::GraphType::EdgeVector());

            mainPred = calculator->orPredicates(mainPred, combinationPredicate);
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> ShortStatePredicateGenerator::makeNodeTubesCombinations_processCombination(
        int nodeId,
        bool makeContainerEq,
        const MachineGraph::GraphType::EdgeVector & aDummyBigger,
        const MachineGraph::GraphType::EdgeVector & aDummySmaller,
        const MachineGraph::GraphType::EdgeVector & lDummyBigger,
        const MachineGraph::GraphType::EdgeVector & lDummySmaller,
        const MachineGraph::GraphType::EdgeVector & aBigger,
        const MachineGraph::GraphType::EdgeVector & aSmaller,
        const MachineGraph::GraphType::EdgeVector & lBigger,
        const MachineGraph::GraphType::EdgeVector & lSmaller)
{
    std::shared_ptr<Predicate> dummySameRateEq = samePropertyTube(aDummyBigger, *calculator.get(), &CommomRulesOperations::calculateRate);
    dummySameRateEq = calculator->andPredicates(dummySameRateEq,
                                                samePropertyTube(aDummySmaller, *calculator.get(), &CommomRulesOperations::calculateRate));
    dummySameRateEq = calculator->andPredicates(dummySameRateEq,
                                                samePropertyTube(lDummyBigger, *calculator.get(), &CommomRulesOperations::calculateRate));
    dummySameRateEq = calculator->andPredicates(dummySameRateEq,
                                                samePropertyTube(lDummySmaller, *calculator.get(), &CommomRulesOperations::calculateRate));

    std::shared_ptr<Predicate> tubesArrivingEq = allTubesSameEquality(aDummyBigger, Equality::bigger, calculator->getNumber(0));
    tubesArrivingEq = calculator->andPredicates(tubesArrivingEq,allTubesSameEquality(aDummySmaller, Equality::lesser, calculator->getNumber(0)));
    tubesArrivingEq = calculator->andPredicates(tubesArrivingEq, allTubesSameEquality(aBigger, Equality::bigger, calculator->getNumber(0)));
    tubesArrivingEq = calculator->andPredicates(tubesArrivingEq, allTubesSameEquality(aSmaller, Equality::lesser, calculator->getNumber(0)));

    std::shared_ptr<Predicate> tubesLeavingEq = allTubesSameEquality(lDummyBigger, Equality::bigger, calculator->getNumber(0));
    tubesLeavingEq = calculator->andPredicates(tubesLeavingEq, allTubesSameEquality(lDummySmaller, Equality::lesser, calculator->getNumber(0)));
    tubesLeavingEq = calculator->andPredicates(tubesLeavingEq, allTubesSameEquality(lBigger, Equality::bigger, calculator->getNumber(0)));
    tubesLeavingEq = calculator->andPredicates(tubesLeavingEq, allTubesSameEquality(lSmaller, Equality::lesser, calculator->getNumber(0)));

    std::shared_ptr<ArithmeticOperable> arrivingIdSum = addAllTubesProperty(aBigger, *calculator.get(), &CommomRulesOperations::calculateId);
    arrivingIdSum = calculator->addOperables(arrivingIdSum, addAllTubesProperty(aDummyBigger, *calculator.get(), &CommomRulesOperations::calculateId));
    arrivingIdSum = calculator->addOperables(arrivingIdSum, addAllTubesProperty(lSmaller, *calculator.get(), &CommomRulesOperations::calculateId));
    arrivingIdSum = calculator->addOperables(arrivingIdSum, addAllTubesProperty(lDummySmaller, *calculator.get(), &CommomRulesOperations::calculateId));

    std::shared_ptr<ArithmeticOperable> arrNonDummyRateSum = addAllTubesProperty(aBigger, *calculator.get(), &CommomRulesOperations::calculateRate);
    arrNonDummyRateSum = calculator->addOperables(arrNonDummyRateSum,
                                                  addAllTubesProperty(lSmaller, *calculator.get(), &CommomRulesOperations::calculateRate));

    std::shared_ptr<ArithmeticOperable> arrDummyRate = NULL;
    if (!aDummyBigger.empty()) {
        MachineGraph::GraphType::EdgeTypePtr tube = *aDummyBigger.begin();
        std::shared_ptr<Variable> tubeVar = calculator->getVariable(VariableNominator::getTubeVarName(tube->getIdSource(),
                                                                                                      tube->getIdTarget()));
        arrDummyRate = calculator->calculateRate(tubeVar);
    } else if (!lDummySmaller.empty()){
        MachineGraph::GraphType::EdgeTypePtr tube = *lDummySmaller.begin();
        std::shared_ptr<Variable> tubeVar = calculator->getVariable(VariableNominator::getTubeVarName(tube->getIdSource(),
                                                                                                      tube->getIdTarget()));
        arrDummyRate = calculator->calculateRate(tubeVar);
    }

    std::shared_ptr<ArithmeticOperable> leavingTubesValue = calculator->addOperables(arrivingIdSum, arrNonDummyRateSum);
    leavingTubesValue = calculator->addOperables(leavingTubesValue, arrDummyRate);

    std::shared_ptr<Predicate> leavingTubesPositiveEq = allTubesSameEquality(lBigger, Equality::equal, leavingTubesValue);
    leavingTubesPositiveEq = calculator->andPredicates(leavingTubesPositiveEq, allTubesSameEquality(lDummyBigger, Equality::equal, leavingTubesValue));

    std::shared_ptr<ArithmeticOperable> negativeLeavingTubesValue = calculator->multiplyOperables(calculator->getNumber(-1), leavingTubesValue);
    std::shared_ptr<Predicate> leavingTubesNegativeEq = allTubesSameEquality(aSmaller, Equality::equal, negativeLeavingTubesValue);
    leavingTubesNegativeEq = calculator->andPredicates(leavingTubesNegativeEq, allTubesSameEquality(aDummySmaller, Equality::equal, negativeLeavingTubesValue));

    std::shared_ptr<Predicate> containerEq = NULL;
    if (makeContainerEq) {
        containerEq = calculator->createEquality(VariableNominator::getContainerVarName(nodeId), leavingTubesValue);
    }

    std::shared_ptr<Predicate> mainPred = calculator->andPredicates(dummySameRateEq, tubesArrivingEq);
    mainPred = calculator->andPredicates(mainPred, tubesLeavingEq);
    mainPred = calculator->andPredicates(mainPred, leavingTubesPositiveEq);
    mainPred = calculator->andPredicates(mainPred, leavingTubesNegativeEq);
    mainPred = calculator->andPredicates(mainPred, containerEq);

    return mainPred;
}

bool ShortStatePredicateGenerator::checkBothDirection(const TubeDirMap & aDirMap,
                                                      const TubeDirMap & lDirMap,
                                                      const MachineGraph::GraphType::EdgeVector & aBigger,
                                                      const MachineGraph::GraphType::EdgeVector & aSmaller,
                                                      const MachineGraph::GraphType::EdgeVector &lBigger,
                                                      const MachineGraph::GraphType::EdgeVector &lSmaller)
{
        bool tubeArrving = false;
        bool tubeLeaving = false;

        if (!aBigger.empty()) {
            tubeArrving = true;
        }
        if(!aSmaller.empty()) {
            tubeLeaving = true;
        }

        if (!(tubeArrving && tubeLeaving)) {
            if (!lBigger.empty()) {
                tubeLeaving = true;
            }
            if (!lSmaller.empty()) {
                tubeArrving = true;
            }
        }

        for(auto it = aDirMap.begin(); !(tubeArrving && tubeLeaving) && it != aDirMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            if (dir == TubeEdge::regular) {
                tubeArrving = true;
            } else if (dir == TubeEdge::inverted) {
                tubeLeaving = true;
            }
        }

        for(auto it = lDirMap.begin(); !(tubeArrving && tubeLeaving) && it != lDirMap.end(); ++it) {
            TubeEdge::TubeDirection dir = it->second;
            if (dir == TubeEdge::regular) {
                tubeLeaving = true;
            } else if (dir == TubeEdge::inverted) {
                tubeArrving = true;
            }
        }

        return (tubeArrving && tubeLeaving);
}



ShortStatePredicateGenerator::LabelTypeTubeMap ShortStatePredicateGenerator::labelNode(int node) {
    LabelTypeTubeMap labelMap;

    labelNodeArrivingLeavingSet(node, true, labelMap);
    labelNodeArrivingLeavingSet(node, false, labelMap);

    return labelMap;
}

ShortStatePredicateGenerator::LabelTypeTubeMap ShortStatePredicateGenerator::getSubMap(const LabelTypeTubeMap & labelMap,
                                                                                       const MachineGraph::GraphType::EdgeVector & tubes)
    throw(std::invalid_argument)
{
    LabelTypeTubeMap labelSubmap;
    for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubes) {
        std::tuple<int,int> tubeId = std::make_tuple(tube->getIdSource(), tube->getIdTarget());

        auto finded = labelMap.find(tubeId);
        if (finded != labelMap.end()) {
            const Label & label = finded->second;
            labelSubmap.insert(std::make_pair(tubeId, label));
        } else {
            throw(std::invalid_argument("ShortStatePredicateGenerator::getSubMap." +
                                        tube->toText() + " does not exists in the map received as argument"));
        }
    }
    return labelSubmap;
}

void ShortStatePredicateGenerator::labelNodeArrivingLeavingSet(int node, bool arriving, LabelTypeTubeMap & labelMap) {
    if (arriving) {
        MachineGraph::GraphType::EdgeVectorPtr tubes = graph->getArrivingTubes(node);
        labelTubeSet(true, *tubes.get(), labelMap);
    } else {
        MachineGraph::GraphType::EdgeVectorPtr tubes = graph->getLeavingTubes(node);
        labelTubeSet(false, *tubes.get(), labelMap);
    }
}

void ShortStatePredicateGenerator::labelTubeSet(bool arriving, const MachineGraph::GraphType::EdgeVector & tubes, LabelTypeTubeMap & labelMap) {
    if (arriving) {
        for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubes) {
            Label tubeLabel = labelTube(tube->getIdSource(), tube->getIdTarget());
            labelMap.insert(std::make_pair(std::make_tuple(tube->getIdSource(), tube->getIdTarget()), tubeLabel));
        }
    } else {
        for(const MachineGraph::GraphType::EdgeTypePtr & tube: tubes) {
            Label tubeLabel = labelTube(tube->getIdTarget(), tube->getIdSource());
            labelMap.insert(std::make_pair(std::make_tuple(tube->getIdSource(), tube->getIdTarget()), tubeLabel));
        }
    }
}

Label ShortStatePredicateGenerator::labelTube(int nodeStart, int nodeAvoid) {
    Label tubeLbel;

    std::unordered_set<int> nodes2avoid = {nodeAvoid};
    MachineGraphPressureIterator graphIt(nodeStart, graph, nodes2avoid);

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
        std::shared_ptr<ArithmeticOperable> firstRate = (operations.*propertyCalculator)(firstTubeVar);
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




















