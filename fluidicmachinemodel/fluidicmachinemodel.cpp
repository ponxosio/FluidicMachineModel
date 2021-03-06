#include "fluidicmachinemodel.h"

FluidicMachineModel::FluidicMachineModel(std::shared_ptr<MachineGraph> graph,
                                         std::shared_ptr<TranslationStack> translationStack,
                                         short int ratePrecisionInteger,
                                         short int ratePrecisionDecimal,
                                         double defaultRate,
                                         units::Volumetric_Flow defaultRateUnits) throw(std::overflow_error) :
    actualFullMachineState(graph, ratePrecisionInteger, ratePrecisionDecimal)
{
    this->graph = graph;
    this->translationStack = translationStack;
    this->defaultRate = defaultRate;
    this->defaultRateUnits = defaultRateUnits;
    maxOpenContainer = actualFullMachineState.getMaxOpenContainers();

    if (graph->getNumOpenContainers() > maxOpenContainer) {
        throw(std::overflow_error("overflow error maximun " + std::to_string(maxOpenContainer) + " open containers exceeded"));
    }
    analizeGraph(ratePrecisionInteger, ratePrecisionDecimal);
}

FluidicMachineModel::~FluidicMachineModel() {

}

bool FluidicMachineModel::canDoMovement(unsigned long mask) {
    unsigned long types = ModelInterface::continuous | ModelInterface::discrete;
    return ((types & mask ) == mask);
}

std::shared_ptr<ComponentInterface> FluidicMachineModel::getComponent(int virtualContainer) throw(std::invalid_argument) {
    return graph->getNode(virtualContainer);
}

void FluidicMachineModel::loadContainer(int id, units::Volume volume) throw(std::invalid_argument) {
    auto finded = graph->getOpenContainerLiquidIdMap().find(id);
    if (finded != graph->getOpenContainerLiquidIdMap().end()) {
        std::shared_ptr<ContainerNode> openContainer = std::dynamic_pointer_cast<ContainerNode>(graph->getNode(id));
        if (openContainer) {
            short int liquid_id = finded->second;
            actualFullMachineState.overrideContainerState(id, std::pow(2, liquid_id));
            openContainer->setActualVolume(volume);
        } else {
            throw(std::invalid_argument("error casting node " + std::to_string(id) + ", possible inconsistency in open_container table."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not an open container"));
    }
}

units::Time FluidicMachineModel::transferLiquid(int sourceId, int targetId,  units::Volume volume) throw(std::invalid_argument) {
    units::Time duration = volume / (defaultRate * defaultRateUnits);
    flowEngine.addFlow(sourceId, targetId, (defaultRate * defaultRateUnits));
    return duration;
}

void FluidicMachineModel::stopTransferLiquid(int sourceId, int targetId) throw(std::invalid_argument) {
    flowEngine.removeFlow(sourceId, targetId);
}

units::Time FluidicMachineModel::mix(
        int sourceId1,
        int sourceId2,
        int targetId,
        units::Volume volume1,
        units::Volume volume2)
    throw(std::invalid_argument)
{
    units::Time duration1 = transferLiquid(sourceId1, targetId, volume1);
    units::Time duration2 = transferLiquid(sourceId2, targetId, volume2);
    return (duration1 > duration2 ? duration1 : duration2);
}

void FluidicMachineModel::stopMix(int sourceId1, int sourceId2, int targetId) throw(std::invalid_argument) {
    stopTransferLiquid(sourceId1, targetId);
    stopTransferLiquid(sourceId2, targetId);
}

void FluidicMachineModel::setContinuousFlow(int idStart, int idEnd, units::Volumetric_Flow flowRate) {
    flowEngine.addFlow(idStart, idEnd, flowRate);
}

void FluidicMachineModel::stopContinuousFlow(int idStart, int idEnd) {
    flowEngine.removeFlow(idStart, idEnd);
}

void FluidicMachineModel::setContinuousFlow(const std::vector<int> & containersIds, units::Volumetric_Flow flowRate) throw(std::invalid_argument)
{
    if (containersIds.size() > 1) {
        int idStart = *containersIds.begin();
        int idEnd = containersIds.back();

        auto findedStart = graph->getOpenContainerLiquidIdMap().find(idStart);
        if (findedStart != graph->getOpenContainerLiquidIdMap().end()) {

            auto findedEnd = graph->getOpenContainerLiquidIdMap().find(idEnd);
            if (findedEnd != graph->getOpenContainerLiquidIdMap().end()) {
                for(auto it = containersIds.begin() + 1; it != containersIds.end(); ++it) {
                    int actualId = *it;
                    flowEngine.addFlow(idStart, actualId, flowRate);
                    idStart = actualId;
                }
            } else {
                throw(std::invalid_argument("FluidicMachineModel::setContinuousFlow(). " + std::to_string(idEnd) + " is not an open container."));
            }
        } else {
            throw(std::invalid_argument("FluidicMachineModel::setContinuousFlow(). " + std::to_string(idStart) + " is not an open container."));
        }
    } else {
        throw(std::invalid_argument("FluidicMachineModel::setContinuousFlow(). containersIds must has at least 2 elements"));
    }
}

void FluidicMachineModel::stopContinuousFlow(const std::vector<int> & containersIds) throw(std::invalid_argument) {
    if (containersIds.size() > 1) {
        int idStart = *containersIds.begin();
        int idEnd = containersIds.back();

        auto findedStart = graph->getOpenContainerLiquidIdMap().find(idStart);
        if (findedStart != graph->getOpenContainerLiquidIdMap().end()) {

            auto findedEnd = graph->getOpenContainerLiquidIdMap().find(idEnd);
            if (findedEnd != graph->getOpenContainerLiquidIdMap().end()) {
                for(auto it = containersIds.begin() + 1; it != containersIds.end(); ++it) {
                    int actualId = *it;
                    flowEngine.removeFlow(idStart, actualId);
                    idStart = actualId;
                }
            } else {
                throw(std::invalid_argument("FluidicMachineModel::stopContinuousFlow(). " + std::to_string(idEnd) + " is not an open container."));
            }
        } else {
            throw(std::invalid_argument("FluidicMachineModel::stopContinuousFlow(). " + std::to_string(idStart) + " is not an open container."));
        }
    } else {
        throw(std::invalid_argument("FluidicMachineModel::stopContinuousFlow(). containersIds must has at least 2 elements"));
    }
}

void FluidicMachineModel::processFlows(const std::vector<int> & containers) throw(std::runtime_error) {
    MachineState containers2Set(actualFullMachineState.getRatePrecisionInteger(), actualFullMachineState.getRatePrecisionDecimal());
    MachineFlow::FlowsVector flows2Set = flowEngine.updateFlows();

    try {
        for(const MachineFlow::PathRateTuple & tuple: flows2Set) {
            addStack2State(std::get<0>(tuple), std::get<1>(tuple), containers2Set);
        }
        setUnabledPumps(containers2Set);
        setUnusedContainersToZero(containers, containers2Set);

        if (!routingEngine) {
            routingEngine = translateRules();
        }
        std::unordered_map<std::string, long long> newState;
        if (routingEngine->calculateNewRoute(containers2Set.getAllContainersTubes(), newState)) {
            actualFullMachineState.setAllStates(newState);
            sendActualState2components();
        } else {
            throw(std::runtime_error("FluidicMachineModel::calculateNewRoute(): imposible to flow: " + flowEngine.flowToStr()));
        }
    } catch (std::invalid_argument & e) {
        throw(std::runtime_error("FluidicMachineModel::claculateNewRoute(): invalid argument error, message:" + std::string(e.what())));
    } catch (std::runtime_error & e) {
        throw(std::runtime_error("FluidicMachineModel::claculateNewRoute():" + std::string(e.what())));
    }
}

void FluidicMachineModel::stopAllOperations() {
    graph->finishAllOperations();
}

bool FluidicMachineModel::checkFlows(
        const std::vector<int> & containers,
        const MachineFlow::FlowsVector & flows2Set)
    throw(std::runtime_error)
{
    bool possible = false;
    MachineState containers2Set(actualFullMachineState.getRatePrecisionInteger(), actualFullMachineState.getRatePrecisionDecimal());

    try {
        for(const MachineFlow::PathRateTuple & tuple: flows2Set) {
            addStack2State(std::get<0>(tuple), std::get<1>(tuple), containers2Set);
        }
        setUnabledPumps(containers2Set);
        setUnusedContainersToZero(containers, containers2Set);

        if (!routingEngine) {
            routingEngine = translateRules();
        }
        std::unordered_map<std::string, long long> newState;
        if (routingEngine->calculateNewRoute(containers2Set.getAllContainersTubes(), newState)) {
            possible = true;
        }
    } catch (std::invalid_argument & ignored) {
        //while checking the flows this exception is ignored, the system will try imposible flows.

    } catch (std::runtime_error & e) {
        throw(std::runtime_error("FluidicMachineModel::checkFlows():" + std::string(e.what())));
    }
    return possible;
}

void FluidicMachineModel::setTranslationStack(std::shared_ptr<TranslationStack> translationStack) {
    this->translationStack = translationStack;
}

void FluidicMachineModel::updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory) {
    this->graph->updatePluginFactory(factory);
}

void FluidicMachineModel::addStack2State(const std::deque<short int> & queue, units::Volumetric_Flow rate, MachineState & state)
    throw(std::invalid_argument)
{
    auto it_start = graph->getOpenContainerLiquidIdMap().find(queue.front());
    if(it_start != graph->getOpenContainerLiquidIdMap().end()) {
        if(graph->getOpenContainerLiquidIdMap().find(queue.back()) != graph->getOpenContainerLiquidIdMap().end()) {
            try {
                short int id = it_start->second;
                units::Volumetric_Flow convertedFlow = rate.to(defaultRateUnits);
                long long stateValue = state.generateState(1 << id, convertedFlow());

                short int containerId = it_start->first;
                state.emplaceContainerVar(containerId);
                state.overrideContainerState(containerId, -stateValue);

                for (auto it = queue.begin() + 1; it != queue.end(); ++it) {
                    state.emplaceContainerVar(*it);
                    state.addContainerState(*it, stateValue);
                }
            } catch (std::overflow_error & e) {
                throw(std::invalid_argument("overflow error, message: " + std::string(e.what())));
            }
        } else {
            throw(std::invalid_argument(std::to_string(queue.back()) + " is not an open container, stacks's last containers mus be an open one."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(queue.front()) + " is not an open container, stacks's frist container must be an open one."));
    }
}

void FluidicMachineModel::sendActualState2components() throw(std::runtime_error) {
    //first stop pumps
    for (const auto tuple: actualFullMachineState.getAllPumpsDirVar()) {
        int id = VariableNominator::getPumpId(tuple.first);
        int dir = tuple.second;
        double rate = (double)actualFullMachineState.getPumpRate(id);

        if (rate == 0) {
            std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
            graphNode->doOperation(Function::pump, 2, dir, rate*defaultRateUnits);
        }
    }

    //second move valves
    for (auto tuple: actualFullMachineState.getAllValves()) {
        int id = VariableNominator::getValveId(tuple.first);
        int pos = tuple.second;

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(Function::route, 1, pos);
    }

    //lastly start pumps
    for (const auto tuple: actualFullMachineState.getAllPumpsDirVar()) {
        int id = VariableNominator::getPumpId(tuple.first);
        int dir = tuple.second;
        double rate = (double)actualFullMachineState.getPumpRate(id);

        if (rate != 0) {
            std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
            graphNode->doOperation(Function::pump, 2, dir, rate*defaultRateUnits);
        }
    }
}

 void FluidicMachineModel::setUnabledPumps(MachineState & machineState) {
     for(int pumpId : disabledPumps) {
         machineState.emplacePumpVar(pumpId);
         machineState.overridePumpDirState(pumpId, 0);
         machineState.overridePumpRateState(pumpId, 0);
     }
 }

 void FluidicMachineModel::setUnusedContainersToZero(const std::vector<int> & containers, MachineState & state) {
    for (int id : containers) {
        if (!state.isContainerPresent(id)) {
            state.emplaceContainerVar(id);
        }
    }
 }

std::shared_ptr<FluidicMachineNode> FluidicMachineModel::getNode(int id) throw(std::invalid_argument) {
    std::shared_ptr<FluidicMachineNode> node = graph->getNode(id);
    if (!node) {
        throw(std::invalid_argument("unknow id " + std::to_string(id)));
    }
    return  node;
}

std::shared_ptr<RoutingEngine> FluidicMachineModel::translateRules() throw(std::runtime_error) {
    if(translationStack) {
        translationStack->clear();
        for(std::shared_ptr<Rule> rule: rules) {
            rule->fillTranslationStack(translationStack.get());
            translationStack->addHeadToRestrictions();
        }
        return std::shared_ptr<RoutingEngine>(translationStack->getRoutingEngine());
    } else {
        throw(std::runtime_error("impossible to trsnalte rules, the translation stack is null"));
    }
}

void FluidicMachineModel::analizeGraph(short int ratePrecisionInteger, short int ratePrecisionDecimal) {
    GraphRulesGenerator generator(graph, ratePrecisionInteger, ratePrecisionDecimal);

    const std::vector<std::shared_ptr<Rule>> & newRules = generator.getRules();
    rules.reserve(newRules.size());
    for(int i = 0; i < newRules.size(); i++) {
        rules.insert(rules.begin() + i, newRules[i]);
    }
}


















