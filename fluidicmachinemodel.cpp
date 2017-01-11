#include "fluidicmachinemodel.h"

FluidicMachineModel::FluidicMachineModel(MachineGraph graph,
                                         std::shared_ptr<TranslationStack> translationStack,
                                         short int ratePrecisionInteger,
                                         short int ratePrecisionDecimal)
{
    this->ratePrecisionDecimal = ratePrecisionDecimal;
    this->ratePrecisionInteger = ratePrecisionInteger;
    this->graph = graph;
    this->translationStack = translationStack;

    analizeGraph();
}

FluidicMachineModel::~FluidicMachineModel() {

}

void FluidicMachineModel::loadContainer(short int id, float volume) throw(std::invalid_argument) {
    auto finded = idOpenContainerLiquidIdMap.find(id);
    if (finded != idOpenContainerLiquidIdMap.end()) {
        std::shared_ptr<ContainerNode> openContainer = std::dynamic_pointer_cast<ContainerNode>(graph->getNode(id));
        if (openContainer) {
            short int liquid_id = finded.second;
            containersTubesInterfaceState.setContainerState(id, std::pow(2, liquid_id));
            actualFullMachineState.setContainerState(id, std::pow(2, liquid_id));
            openContainer->setActualVolume(volume);
        } else {
            throw(std::invalid_argument("error casting node " + std::to_string(id) + ", possible inconsistency in open_container table."));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not an open container"));
    }
}

void FluidicMachineModel::setContinuousFlow(short int idStart, short int idEnd, float flowRate,
                                            const std::vector<short int> * intermediateContainers,
                                            const std::vector<std::tuple<short int, short int, bool>> * intermediateTubes) throw(std::invalid_argument)
{
    auto itIdLiquidStart = idOpenContainerLiquidIdMap.find(idStart);
    if (itIdLiquidStart != idOpenContainerLiquidIdMap.end()) {
        auto itIdLiquidEnd = idOpenContainerLiquidIdMap.find(idStart);
        if (itIdLiquidEnd != idOpenContainerLiquidIdMap.end()) {
            try {
                short int flowRateInt = transformRateToInt(flowRate);
                short int idLiquidStart = * itIdLiquidStart;
                short int idLiquidEnd = * itIdLiquidEnd;
                long long startState = -(std::pow(10, ratePrecisionDecimal + ratePrecisionInteger) * idLiquidStart + flowRateInt);

                addToContainersState(idEnd,-startState);

                if (intermediateContainers) {
                    for (short int idContainer: *intermediateContainers) {
                        addToContainersState(idContainer, -startState);
                    }
                }

                if (intermediateTubes) {
                    for(std::tuple<short int, short int, bool> tubeId: *intermediateTubes) {
                        int dir = std::get<2>(tubeId) ? -1 : 1;
                        addToTubeState(std::get<0>(tubeId), std::get<1>(tubeId), dir * startState);
                    }
                }


            } catch (std::invalid_argument & e) {
                throw(std::invalid_argument("error while setting flow between, " + std::to_string(idSource) + "_" + std::to_string(idTarget) +
                                            ". Error message: " + e.what()));
            }
        } else {
            throw (std::invalid_argument(std::to_string(idEnd) +  " is not an open container"));
        }
    } else {
        throw (std::invalid_argument(std::to_string(idStart) +  " is not an open container"));
    }
}
