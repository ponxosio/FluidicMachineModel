#include "fluidicmachinemodel.h"

FluidicMachineModel::FluidicMachineModel(std::shared_ptr<MachineGraph> graph,
                                         std::shared_ptr<TranslationStack> translationStack,
                                         short int ratePrecisionInteger,
                                         short int ratePrecisionDecimal) throw(std::overflow_error) :
    actualFullMachineState(graph, ratePrecisionDecimal, ratePrecisionInteger)
{
    this->graph = graph;
    this->translationStack = translationStack;
    maxOpenContainer = actualFullMachineState.getMaxOpenContainers();

    try {
        analizeGraph();
    } catch (std::overflow_error & e) {
        throw(std::overflow_error("overflow error while analyzing graph, error: " + std::string(e.what())));
    }
}

FluidicMachineModel::~FluidicMachineModel() {

}

void FluidicMachineModel::loadContainer(short int id, float volume) throw(std::invalid_argument) {
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

void FluidicMachineModel::setContinuousFlow(short int idStart, short int idEnd, float flowRate) throw(std::invalid_argument)
{
    auto findedStart = graph->getOpenContainerLiquidIdMap().find(idStart);
    if (findedStart != graph->getOpenContainerLiquidIdMap().end()) {
        auto findedEnd = graph->getOpenContainerLiquidIdMap().find(idEnd);
        if (findedEnd != graph->getOpenContainerLiquidIdMap().end()) {
            flowEngine.addFlow(idStart, idEnd, flowRate);
        } else {
            throw(std::invalid_argument(std::to_string(idEnd) + " is not an open container."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idStart) + " is not an open container."));
    }
}

void FluidicMachineModel::stopContinuousFlow(short int idStart, short int idEnd) throw(std::invalid_argument) {
    auto findedStart = graph->getOpenContainerLiquidIdMap().find(idStart);
    if (findedStart != graph->getOpenContainerLiquidIdMap().end()) {
        auto findedEnd = graph->getOpenContainerLiquidIdMap().find(idEnd);
        if (findedEnd != graph->getOpenContainerLiquidIdMap().end()) {
            flowEngine.removeFlow(idStart, idEnd);
        } else {
            throw(std::invalid_argument(std::to_string(idEnd) + " is not an open container."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idStart) + " is not an open container."));
    }
}

void FluidicMachineModel::calculateNewRoute() throw(std::runtime_error) {
    MachineState containers2Set(graph, actualFullMachineState.getRatePrecisionInteger(), actualFullMachineState.getRatePrecisionDecimal());
    MachineFlow::FlowsVector flows2Set = flowEngine.updateFlows();

    try {
        for(const MachineFlow::PathRateTuple & tuple: flows2Set) {
            addStack2State(std::get<0>(tuple), std::get<1>(tuple), containers2Set);
        }
        if (!routingEngine) {
            routingEngine = translateRules();
        }
        actualFullMachineState.setAllStates(routingEngine->calculateNewRoute(containers2Set.getAllContainersTubes()));
        sendActualState2components();
    } catch (std::invalid_argument & e) {
        throw(std::runtime_error("claculateNewRoute(): invalid argument error, message:" + std::string(e.what())));
    } catch (std::runtime_error & e) {
        throw(std::runtime_error("claculateNewRoute():" + std::string(e.what())));
    }
}

void FluidicMachineModel::addStack2State(const std::deque<short int> & queue, float rate, MachineState & state) throw(std::invalid_argument) {
    auto it_start = graph->getOpenContainerLiquidIdMap().find(queue.front());
    if(it_start != graph->getOpenContainerLiquidIdMap().end()) {
        if(graph->getOpenContainerLiquidIdMap().find(queue.back()) != graph->getOpenContainerLiquidIdMap().end()) {
            try {
                short int id = it_start->second;
                long long stateValue = state.generateState(id, rate);
                state.overrideContainerState(id, -stateValue);

                for (auto it = queue.begin() + 1; it != queue.end(); ++it) {
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
        float rate = actualFullMachineState.getPumpRate(id);

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(pump, 2, dir, rate);
    }

    for (auto tuple: actualFullMachineState.getAllValves()) {
        int id = VariableNominator::getValveId(tuple.first);
        int pos = tuple.second;

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(route, 1, pos);
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

void FluidicMachineModel::analizeGraph() throw(std::overflow_error) {

}
