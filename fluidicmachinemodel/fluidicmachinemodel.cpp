#include "fluidicmachinemodel.h"

FluidicMachineModel::FluidicMachineModel(std::shared_ptr<MachineGraph> graph,
                                         std::shared_ptr<TranslationStack> translationStack,
                                         short int ratePrecisionInteger,
                                         short int ratePrecisionDecimal) throw(std::overflow_error) :
    actualFullMachineState(graph, ratePrecisionInteger, ratePrecisionDecimal)
{
    this->graph = graph;
    this->translationStack = translationStack;
    defaultRateUtis = units::ml / units::hr;
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

std::shared_ptr<MappingInterface> FluidicMachineModel::findProtocolRelation(std::shared_ptr<ProtocolGraph> protocol) throw(std::invalid_argument) {
    //TODO:
    return NULL;
}

std::shared_ptr<ComponentInterface> FluidicMachineModel::getComponent(int virtualContainer) throw(std::invalid_argument) {
    //TODO:
    return NULL;
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

void FluidicMachineModel::transferLiquid(int sourceId, int targetId,  units::Volume volume) throw(std::invalid_argument) {
    //TODO:
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

void FluidicMachineModel::processFlows() throw(std::runtime_error) {
    MachineState containers2Set(actualFullMachineState.getRatePrecisionInteger(), actualFullMachineState.getRatePrecisionDecimal());
    MachineFlow::FlowsVector flows2Set = flowEngine.updateFlows();

    try {
        for(const MachineFlow::PathRateTuple & tuple: flows2Set) {
            addStack2State(std::get<0>(tuple), std::get<1>(tuple), containers2Set);
        }
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

bool FluidicMachineModel::checkFlows(const MachineFlow::FlowsVector & flows2Set) throw(std::runtime_error) {
    bool possible = false;
    MachineState containers2Set(actualFullMachineState.getRatePrecisionInteger(), actualFullMachineState.getRatePrecisionDecimal());

    try {
        for(const MachineFlow::PathRateTuple & tuple: flows2Set) {
            addStack2State(std::get<0>(tuple), std::get<1>(tuple), containers2Set);
        }
        if (!routingEngine) {
            routingEngine = translateRules();
        }
        std::unordered_map<std::string, long long> newState;
        if (routingEngine->calculateNewRoute(containers2Set.getAllContainersTubes(), newState)) {
            possible = true;
        }
    } catch (std::invalid_argument & e) {
        throw(std::runtime_error("FluidicMachineModel::checkFlows(): invalid argument error, message:" + std::string(e.what())));
    } catch (std::runtime_error & e) {
        throw(std::runtime_error("FluidicMachineModel::checkFlows():" + std::string(e.what())));
    }
    return possible;
}

void FluidicMachineModel::addStack2State(const std::deque<short int> & queue, units::Volumetric_Flow rate, MachineState & state)
    throw(std::invalid_argument)
{
    auto it_start = graph->getOpenContainerLiquidIdMap().find(queue.front());
    if(it_start != graph->getOpenContainerLiquidIdMap().end()) {
        if(graph->getOpenContainerLiquidIdMap().find(queue.back()) != graph->getOpenContainerLiquidIdMap().end()) {
            try {
                short int id = it_start->second;
                units::Volumetric_Flow convertedFlow = rate.to(defaultRateUtis);
                long long stateValue = state.generateState(1 << id, convertedFlow());

                state.emplaceContainerVar(id);
                state.overrideContainerState(id, -stateValue);

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
     for (const auto tuple: actualFullMachineState.getAllPumpsDirVar()) {
        int id = VariableNominator::getPumpId(tuple.first);
        int dir = tuple.second;
        double rate = (double)actualFullMachineState.getPumpRate(id);

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(Function::pump, 2, dir, rate*defaultRateUtis);
    }

    for (auto tuple: actualFullMachineState.getAllValves()) {
        int id = VariableNominator::getValveId(tuple.first);
        int pos = tuple.second;

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(Function::route, 1, pos);
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


















