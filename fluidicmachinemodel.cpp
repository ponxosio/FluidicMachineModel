#include "fluidicmachinemodel.h"

FluidicMachineModel::FluidicMachineModel(MachineGraph graph,
                                         std::shared_ptr<TranslationStack> translationStack,
                                         short int ratePrecisionInteger,
                                         short int ratePrecisionDecimal) throw(std::overflow_error) :
    actualFullMachineState(ratePrecisionDecimal, ratePrecisionInteger)
{
    this->graph = graph;
    this->translationStack = translationStack;
    maxOpenContainer = calculateMaxOpenContainers(ratePrecisionDecimal, ratePrecisionInteger);

    try {
        analizeGraph();
    } catch (std::overflow_error & e) {
        throw(std::overflow_error("overflow error while analyzing graph, error: " + e.what()));
    }
}

FluidicMachineModel::~FluidicMachineModel() {

}

void FluidicMachineModel::loadContainer(short int id, float volume) throw(std::invalid_argument) {
    auto finded = idOpenContainerLiquidIdMap.find(id);
    if (finded != idOpenContainerLiquidIdMap.end()) {
        std::shared_ptr<ContainerNode> openContainer = std::dynamic_pointer_cast<ContainerNode>(graph->getNode(id));
        if (openContainer) {
            short int liquid_id = finded.second;
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
    auto findedStart = idOpenContainerLiquidIdMap.find(idStart);
    if (findedStart != idOpenContainerLiquidIdMap.end()) {
        auto findedEnd = idOpenContainerLiquidIdMap.find(idEnd);
        if (findedEnd != idOpenContainerLiquidIdMap.end()) {
            flowEngine.addFlow(idStart, idEnd, flowRate);
        } else {
            throw(std::invalid_argument(std::to_string(idEnd) + " is not an open container."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idStart) + " is not an open container."));
    }
}

void FluidicMachineModel::stopContinuousFlow(short int idStart, short int idEnd) throw(std::invalid_argument) {
    auto findedStart = idOpenContainerLiquidIdMap.find(idStart);
    if (findedStart != idOpenContainerLiquidIdMap.end()) {
        auto findedEnd = idOpenContainerLiquidIdMap.find(idEnd);
        if (findedEnd != idOpenContainerLiquidIdMap.end()) {
            flowEngine.removeFlow(idStart, idEnd);
        } else {
            throw(std::invalid_argument(std::to_string(idEnd) + " is not an open container."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idStart) + " is not an open container."));
    }
}

void FluidicMachineModel::calculateNewRoute() throw(std::runtime_error) {
    MachineState containers2Set;
    MachineFlow::FlowsVector flows2Set = flowEngine.updateFlows();

    try {
        for(const MachineFlow::PathRateTuple & tuple: flows2Set) {
            addStack2State(std::get<0>(tuple), rate, containers2Set);
        }
        if (!routingEngine) {
            routingEngine = translateRules();
        }
        actualFullMachineState.setAllStates(routingEngine->calculateNewRoute(containers2Set.getAllContainersTubes()));
        sendState2components(actualFullMachineState);
    } catch (std::invalid_argument & e) {
        throw(std::runtime_error("claculateNewRoute(): invalid argument error, message:" + e.what() ));
    } catch (std::runtime_error & e) {
        throw(std::runtime_error("claculateNewRoute():" + e.what()));
    }
}

void FluidicMachineModel::addStack2State(const std::deque<short int> & queue, float rate, MachineState & state) throw(std::invalid_argument) {
    auto it_start = idOpenContainerLiquidIdMap.find(queue.front());
    if(it_start != idOpenContainerLiquidIdMap.end()) {
        if(idOpenContainerLiquidIdMap.find((queue.back()) != idOpenContainerLiquidIdMap.end())) {
            try {
                short int id = *it;
                long long stateValue = state.generateState(id, rate);
                state.overrideContainerState(id, -stateValue);

                for (auto it = queue.begin() + 1; it != queue.end(); ++it) {
                    state.addContainerState(*it, stateValue);
                }
            } catch (std::overflow_error & e) {
                throw(std::invalid_argument("overflow error, message: " + e.what()));
            }
        } else {
            throw(std::invalid_argument(std::to_string(queue.back()) + " is not an open container"));
        }
    } else {
        throw(std::invalid_argument(std::to_string(queue.front()) + " is not an open container"));
    }
}

 void FluidicMachineModel::sendState2components(const MachineState & state) throw(std::runtime_error) {
    for (auto tuple: state.getAllPumpsDirVar()) {
        int id = state.getPumpId(tuple.first);
        int dir = tuple.second;
        float rate = state.getPumpRate(id);

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(pump, 2, dir, rate);
    }

    for (auto tuple: state.getAllValves()) {
        int id = state.getValveId(tuple.first);
        int pos = state.second;

        std::shared_ptr<FluidicMachineNode> graphNode = graph->getNode(id);
        graphNode->doOperation(route, 1, pos);
    }
 }

short int FluidicMachineModel::calculateMaxOpenContainers(short int ratePrecisionDecimal, short int ratePrecisionInteger) {
    float max = std::pow(10.0, LONG_LONG_DIGITS_SIZE - (ratePrecisionDecimal + ratePrecisionInteger)) - 1.0;
    return std::round(std::log2f(max));
}
