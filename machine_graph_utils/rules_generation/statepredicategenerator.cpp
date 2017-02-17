#include "statepredicategenerator.h"

#include "machine_graph_utils/graphrulesgenerator.h"
#include "machine_graph_utils/rules_generation/commomrulesoperations.h"

StatePredicateGenerator::StatePredicateGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator)
    throw(std::invalid_argument)
{
    try {
        this->graph = graph;
        this->calculator = calculator;

        std::shared_ptr<Predicate> closePredicate = generatePumpsCloseState();
        std::shared_ptr<Predicate> validStates = generateValidStates();

        if (validStates == NULL) {
            throw(std::invalid_argument("The graph does not has at least one valid state."));
        }

        rules.push_back(calculator->orPredicates(closePredicate, validStates));
    } catch (std::exception & e) {
        throw(std::invalid_argument("expeption while analyzing graph. Message: " + std::string(e.what())));
    }
}

StatePredicateGenerator::~StatePredicateGenerator() {

}

std::shared_ptr<Predicate> StatePredicateGenerator::generateValidStates() throw(std::invalid_argument) {
    MachineGraphGenerator generator(graph);
    try {
        std::shared_ptr<Predicate> mainPred = NULL;
        while(generator.calculateNext()) {
            std::shared_ptr<MachineGraph> actualValidMachine = generator.getActualGraphGenerated();

            std::shared_ptr<Predicate> statePredicate = generatePredicateActualState(generator);

            std::shared_ptr<Predicate> componentsPredicate = NULL;
            for (const std::shared_ptr<MachineGraph> & component: actualValidMachine->getConnectedComponents()) {
                std::shared_ptr<Predicate> tempComponentPredicate = generatePredicateConnectedComponent(component);
                componentsPredicate = calculator->andPredicates(componentsPredicate, tempComponentPredicate);
            }

            std::shared_ptr<Predicate> fullState = calculator->andPredicates(statePredicate, componentsPredicate);
            mainPred = calculator->orPredicates(mainPred, fullState);
        }
        return mainPred;
    } catch (std::invalid_argument & e) {
        std::invalid_argument("Exception while generating rules for state: " + generator.getStateStr() +
                              ".Error message: " + std::string(e.what()));
    }
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicateActualState(MachineGraphGenerator & generator) {
    std::shared_ptr<Predicate> mainPred = NULL;
    const std::vector<std::tuple<int, int>> & sequenceVector = generator.getActualStateVector();
    int pumpIndex = generator.getMaxPumpsIndex();

    for (int i = 0; i < sequenceVector.size(); i++) {
        std::shared_ptr<Predicate> actualPred = NULL;
        const std::tuple<int, int> & actualTuple = sequenceVector[i];
        if (i < pumpIndex) {
            int pumpId = std::get<0>(actualTuple);
            int dir = std::get<1>(actualTuple);
            actualPred = calculator->createEquality(VariableNominator::getPumpVarName(pumpId), dir);
        } else {
            int valveId = std::get<0>(actualTuple);
            int position = std::get<1>(actualTuple);
            actualPred = calculator->createEquality(VariableNominator::getValveVarName(valveId), position);
        }
        mainPred = calculator->andPredicates(mainPred, actualPred);
    }
    return mainPred;
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicateConnectedComponent(std::shared_ptr<MachineGraph> component) throw(std::invalid_argument) {
    try {
        std::shared_ptr<Predicate> mainPred = NULL;

        for(auto pair: *component->getAllNodesMap().get()) {
            MachineGraph::GraphType::EdgeVector directionLeaving;
            MachineGraph::GraphType::EdgeVector directionArriving;
            int nodeId = pair.first;

            std::shared_ptr<Predicate> tubeEqualities = generatePredicateConnectedComponent_processTubes(nodeId, component, directionLeaving, directionArriving);
            std::shared_ptr<ArithmeticOperable> arrivingTubesSum = calculatedVectorTubeSum(directionArriving);

            std::shared_ptr<Predicate> leavingEdgesEquality = NULL;
            if (arrivingTubesSum != NULL) {
                std::shared_ptr<Predicate> rateEqPredicate = generatePredicateSameRateTube(directionArriving);
                leavingEdgesEquality = calculateLeavingTubeValue(directionLeaving, arrivingTubesSum);
                leavingEdgesEquality = calculator->andPredicates(rateEqPredicate, leavingEdgesEquality);
            } else if (!component->isOpenContainer(nodeId)) {
                std::shared_ptr<ArithmeticOperable> zero = calculator->getNumber(0);
                leavingEdgesEquality = calculateLeavingTubeValue(directionLeaving, zero);
            }

            std::shared_ptr<Predicate> specificNodePredicates = NULL;
            if (component->isOpenContainer(nodeId)) {
                specificNodePredicates = generatePredicateOpenContainer(nodeId, component);
            } else if (component->isCloseContainer(nodeId)) {
                specificNodePredicates = generatePredicateCloseContainer(nodeId, arrivingTubesSum);
            } else if (component->isPump(nodeId)) {
                specificNodePredicates = generatePredicatePump(nodeId, directionLeaving, directionArriving);
            }
            mainPred = calculator->andPredicates(mainPred, calculator->andPredicates(calculator->andPredicates(tubeEqualities, leavingEdgesEquality),specificNodePredicates));
        }
        return mainPred;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("expeption while generating rules for connected component: " + component->toString() +
                                    ". Message: " + std::string(e.what())));
    }
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicateConnectedComponent_processTubes(int nodeId,
                                                             std::shared_ptr<MachineGraph> graph,
                                                             MachineGraph::GraphType::EdgeVector & leaving,
                                                             MachineGraph::GraphType::EdgeVector & arriving)
{
    std::shared_ptr<Predicate> edgePred = NULL;
    MachineGraph::GraphType::EdgeVectorPtr actualLeaving = graph->getLeavingTubes(nodeId);
    for(MachineGraph::GraphType::EdgeTypePtr tube: *actualLeaving.get()) {
        std::string tubeName = VariableNominator::getTubeVarName(tube->getIdSource(),tube->getIdTarget());
        if (tube->isCutted()) {
            std::shared_ptr<Predicate> cutEquality = calculator->createEquality(tubeName, 0);
            edgePred = calculator->andPredicates(edgePred, cutEquality);
        } else {
            if (tube->getDirection() == TubeEdge::inverted) {
                std::shared_ptr<Predicate> lessEq = calculator->createLesserEquals(tubeName, 0);
                edgePred = calculator->andPredicates(edgePred, lessEq);

                arriving.push_back(tube);
            } else {
                std::shared_ptr<Predicate> bigEq = calculator->createBiggerEquals(tubeName, 0);
                edgePred = calculator->andPredicates(edgePred, bigEq);

                leaving.push_back(tube);
            }
        }
    }

    MachineGraph::GraphType::EdgeVectorPtr actualArriving = graph->getArrivingTubes(nodeId);
    for(MachineGraph::GraphType::EdgeTypePtr tube: *actualArriving.get()) {
        std::string tubeName = VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget());
        if (tube->isCutted()) {
            std::shared_ptr<Predicate> cutEquality = calculator->createEquality(tubeName,0);
            edgePred = calculator->andPredicates(edgePred, cutEquality);
        } else {
            if (tube->getDirection() == TubeEdge::inverted) {
                std::shared_ptr<Predicate> lessEq = calculator->createLesserEquals(tubeName, 0);
                edgePred = calculator->andPredicates(edgePred, lessEq);

                leaving.push_back(tube);
            } else {
                std::shared_ptr<Predicate> bigEq = calculator->createBiggerEquals(tubeName, 0);
                edgePred = calculator->andPredicates(edgePred, bigEq);

                arriving.push_back(tube);
            }
        }
    }
    return edgePred;
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicateSameRateTube(const MachineGraph::GraphType::EdgeVector & arriving) {
    std::shared_ptr<Predicate> mainPred = NULL;
    if (arriving.size() > 1) {
        std::shared_ptr<ArithmeticOperable> lastRate = NULL;
        std::shared_ptr<ArithmeticOperable> actualRate = NULL;

        for(auto it = arriving.begin(); it != arriving.end(); ++it) {
            MachineGraph::GraphType::EdgeTypePtr actualTube = *it;
            if (!actualTube->isCutted()) {
                std::shared_ptr<Variable> actualTubeVar = calculator->getVariable(VariableNominator::getTubeVarName(actualTube->getIdSource(), actualTube->getIdTarget()));
                if (lastRate == NULL) {
                    lastRate = calculator->calculateRate(actualTubeVar);
                } else {
                    actualRate = calculator->calculateRate(actualTubeVar);

                    std::shared_ptr<Predicate> tempPred = calculator->createEquality(lastRate, actualRate);
                    mainPred = calculator->andPredicates(mainPred, tempPred);
                    lastRate = actualRate;
                }
            }
        }
    }
    return mainPred;
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicateCloseContainer(int nodeId, std::shared_ptr<ArithmeticOperable> arrivingSum) {
    if (arrivingSum == NULL) {
        return calculator->createEquality(VariableNominator::getContainerVarName(nodeId), calculator->getNumber(0));
    } else {
        return calculator->createEquality(VariableNominator::getContainerVarName(nodeId), arrivingSum);
    }
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicateOpenContainer(int nodeId, std::shared_ptr<MachineGraph> component) {
    std::shared_ptr<Predicate> pred = NULL;

    MachineGraph::GraphType::EdgeVectorPtr leaving = component->getLeavingTubes(nodeId);
    MachineGraph::GraphType::EdgeVectorPtr arriving = component->getArrivingTubes(nodeId);
    if (!leaving->empty()) {
        std::shared_ptr<ArithmeticOperable> leavingSum = calculatedVectorTubeSum(*leaving.get());
        if (leavingSum != NULL) {   
            leavingSum = std::make_shared<BinaryOperation>(calculator->getNumber(-1), BinaryOperation::multiply, leavingSum);
            pred = calculator->createEquality(VariableNominator::getContainerVarName(nodeId), leavingSum);

            std::shared_ptr<Predicate> sameRate = generatePredicateSameRateTube(*leaving.get());
            pred = calculator->andPredicates(sameRate, pred);
        } else {
            leavingSum = calculator->getNumber(0);
            pred = calculator->createEquality(VariableNominator::getContainerVarName(nodeId), leavingSum);
        }
    } else if (!arriving->empty()) {
        std::shared_ptr<ArithmeticOperable> arrivingSum = calculatedVectorTubeSum(*arriving.get());
        if (arrivingSum == NULL) {
            arrivingSum = calculator->getNumber(0);
            pred = calculator->createEquality(VariableNominator::getContainerVarName(nodeId), arrivingSum);
        } else {
            pred = calculator->createEquality(VariableNominator::getContainerVarName(nodeId), arrivingSum);

            std::shared_ptr<Predicate> sameRate = generatePredicateSameRateTube(*arriving.get());
            pred = calculator->andPredicates(sameRate, pred);
        }
    } else {
        pred = calculator->createEquality(VariableNominator::getContainerVarName(nodeId), calculator->getNumber(0));
    }
    return pred;
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePredicatePump(int pumpId,
                                                                          const MachineGraph::GraphType::EdgeVector & leaving,
                                                                          const MachineGraph::GraphType::EdgeVector & arriving)
{
    std::shared_ptr<Predicate> mainPred = NULL;
    MachineGraph::GraphType::EdgeTypePtr tubeForRate = NULL;

    for(MachineGraph::GraphType::EdgeTypePtr tube: arriving) {
        if(!tube->isCutted()) {
            if (tubeForRate == NULL) {
                tubeForRate = tube;
            }
            std::shared_ptr<Predicate> eq = calculator->createNotEqual(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()), 0);
            mainPred = calculator->andPredicates(mainPred, eq);
        }
    }
    for(MachineGraph::GraphType::EdgeTypePtr tube: leaving) {
        if(!tube->isCutted()) {
            if (tubeForRate == NULL) {
                tubeForRate = tube;
            }
            std::shared_ptr<Predicate> eq = calculator->createNotEqual(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()), 0);
            mainPred = calculator->andPredicates(mainPred, eq);
        }
    }

    std::shared_ptr<ArithmeticOperable> rate = NULL;
    if (tubeForRate != NULL) {
        std::shared_ptr<Variable> tubeVar = calculator->getVariable(VariableNominator::getTubeVarName(tubeForRate->getIdSource(), tubeForRate->getIdTarget()));
        rate = calculator->calculateRate(tubeVar);
    } else {
        rate = calculator->getNumber(0);
    }

    std::shared_ptr<ArithmeticOperable> rateVar = calculator->getVariable(VariableNominator::getPumpRateVarName(pumpId));
    std::shared_ptr<Predicate> pumpRate = std::make_shared<Equality>(rateVar, Equality::equal, rate);
    mainPred = calculator->andPredicates(mainPred, pumpRate);
    return mainPred;
}

std::shared_ptr<Predicate> StatePredicateGenerator::calculateLeavingTubeValue(const MachineGraph::GraphType::EdgeVector & leaving,
                                                                          std::shared_ptr<ArithmeticOperable> arrivingSum)
{
    std::shared_ptr<Predicate> mainPredicate = NULL;
    for(MachineGraph::GraphType::EdgeTypePtr tube: leaving) {
        if (!tube->isCutted()) {
            std::shared_ptr<Predicate> tubeEq = calculator->createEquality(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()), arrivingSum);
            mainPredicate = calculator->andPredicates(mainPredicate, tubeEq);
        }
    }
    return mainPredicate;
}

std::shared_ptr<ArithmeticOperable> StatePredicateGenerator::calculatedVectorTubeSum(const MachineGraph::GraphType::EdgeVector & vector) {
    std::shared_ptr<ArithmeticOperable> sum = NULL;
    if (!vector.empty()) {
        if (vector.size() == 1) {
            MachineGraph::GraphType::EdgeTypePtr edge = *vector.begin();
            sum = calculator->getVariable(VariableNominator::getTubeVarName(edge->getIdSource(), edge->getIdTarget()));
        } else {
            std::shared_ptr<ArithmeticOperable> firstRate = NULL;

            for(MachineGraph::GraphType::EdgeTypePtr tube: vector) {
                std::shared_ptr<ArithmeticOperable> tempSum = NULL;
                std::shared_ptr<Variable> varName = calculator->getVariable(VariableNominator::getTubeVarName(tube->getIdSource(), tube->getIdTarget()));
                if (!tube->isCutted()) {
                    tempSum = calculator->calculateId(varName);
                    if (firstRate == NULL) {
                        firstRate = calculator->calculateRate(varName);
                    }
                } else {
                    tempSum = varName;
                }
                sum = calculator->addOperables(sum, tempSum);
            }
            sum = calculator->addOperables(sum, firstRate);
        }
    }
    return sum;
}

std::set<int> StatePredicateGenerator::getValvePossibleValues(int id) throw(std::invalid_argument) {
    try {
        std::shared_ptr<ValveNode> valvePtr = graph->getValve(id);
        return valvePtr->getValvePossibleValues();
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("impossible to get valve's values. Error: " + std::string(e.what())));
    }
}

std::shared_ptr<Predicate> StatePredicateGenerator::generatePumpsCloseState() {
    auto it_pump = graph->getPumpsIdsSet().begin();
    std::shared_ptr<Predicate> mainPred = calculator->createEquality(VariableNominator::getPumpVarName(*it_pump), 0);
    mainPred = calculator->andPredicates(mainPred, calculator->createEquality(VariableNominator::getPumpRateVarName(*it_pump), 0));
    ++it_pump;

    for(; it_pump != graph->getPumpsIdsSet().end(); ++it_pump) {
        std::shared_ptr<Predicate> tempPred = calculator->createEquality(VariableNominator::getPumpVarName(*it_pump), 0);
        mainPred = calculator->andPredicates(mainPred, tempPred);

        tempPred = calculator->createEquality(VariableNominator::getPumpRateVarName(*it_pump), 0);
        mainPred = calculator->andPredicates(mainPred, tempPred);
    }

    for(int valveId : graph->getValvesIdsSet()) {
        std::string name = VariableNominator::getValveVarName(valveId);

        std::set<int> positions = getValvePossibleValues(valveId);
        auto itPos = positions.begin();
        std::shared_ptr<Predicate> tempPred = calculator->createEquality(name, *itPos);
        itPos++;

        for (; itPos != positions.end(); ++itPos) {
            std::shared_ptr<Predicate> actualPosEq = calculator->createEquality(name, *itPos);
            tempPred = calculator->orPredicates(tempPred, actualPosEq);
        }

        mainPred = calculator->andPredicates(mainPred, tempPred);
    }

    for(int closeId : graph->getCloseContainersIdsSet()) {
        std::shared_ptr<Predicate> tempPred = calculator->createEquality(VariableNominator::getContainerVarName(closeId), 0);
        mainPred = calculator->andPredicates(mainPred, tempPred);
    }

    for(int openId : graph->getOpenContainersIdsSet()) {
        std::shared_ptr<Predicate> tempPred = calculator->createEquality(VariableNominator::getContainerVarName(openId), 0);
        mainPred = calculator->andPredicates(mainPred, tempPred);
    }

    for(auto tube_pair : *graph->getAlledgesMap().get()) {
        MachineGraph::GraphType::EdgeTypePtr edgePtr = tube_pair.second;
        std::shared_ptr<Predicate> tempPred = calculator->createEquality(VariableNominator::getTubeVarName(edgePtr->getIdSource(), edgePtr->getIdTarget()), 0);
        mainPred = calculator->andPredicates(mainPred, tempPred);
    }

    return mainPred;
}
