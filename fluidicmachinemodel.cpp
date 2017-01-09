#include "fluidicmachinemodel.h"


FluidicMachineModel::FluidicMachineModel(MachineGraph graph, std::shared_ptr<TranslationStack> translationStack) {
    this->graph = graph;
    this->translationStack = translationStack;
    createRules();
}

FluidicMachineModel::~FluidicMachineModel() {

}

void FluidicMachineModel::loadContainer(int id, double volume) throw(std::invalid_argument) {
    std::string varName = "C" + std::to_string(id);

    auto it = containerStatusMap.find(varName);
    if (it != containerStatusMap.end()) {
        it->second = pow(2,(id+1));
        std::shared_ptr<ContainerNode> node = std::dynamic_pointer_cast<ContainerNode>(graph->getNode(id));
        if (node) {
            node->setActualVolume(volume);
        } else {
            throw(std::invalid_argument("error getting " + std::to_string(id) + " node, id does not exists"));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + ", does not exists as a container"));
    }
}

void FluidicMachineModel::setContinuousFlow(int idSource, int idTarget, double flowRate) {
    std::string varCSource = "C" + std::to_string(idSource);
    std::string varCTarget = "C" + std::to_string(idTarget);

    auto itSource = containerStatusMap.find(varCSource);
    if (itSource != containerStatusMap.end()) {
        if (*itSource->second != UNDEFINE_VAR_STATUS) {
            auto itTarget = containerStatusMap.find(varCTarget);
            if (itTarget != containerStatusMap.end()) {
                itTarget->second += itSource->second;

                std::vector<std::tuple<std::string, int>> newStates;
                if (calculateNewRoute(newStates)) {
                    setNewMachineState(newStates, flowRate);
                } else {
                    throw(std::invalid_argument("imposible to route"));
                }
            } else {
                throw(std::invalid_argument(std::to_string(idTarget) + ", does not exists as a container"));
            }
        } else {
            throw(std::invalid_argument(std::to_string(idSource) + ", does not has any liquid"));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idSource) + ", does not exists as a container"));
    }
}

void FluidicMachineModel::stopContinuousFlow(int idSource, int idTarget) {
    std::string varCSource = "C" + std::to_string(idSource);
    std::string varCTarget = "C" + std::to_string(idTarget);

    auto itTarget = containerStatusMap.find(varCSource);
    auto itSource = containerStatusMap.find(varCTarget);

    if (itSource != containerStatusMap.end()) {
        if (itTarget != containerStatusMap.end()) {
            if (((*itSource->second) != UNDEFINE_VAR_STATUS) &&
                    ((*itTarget->second) != UNDEFINE_VAR_STATUS))
            {
                unsigned int sourceFlag = (unsigned int) *itSource->second;
                unsigned int targetFlag = (unsigned int) *itTarget->second;
                if ((sourceFlag & targetFlag) == sourceFlag) {
                    itTarget->second -= itSource->second;
                    if (itTarget->second == 0) {
                        itTarget = -1;
                    }

                    std::vector<std::tuple<std::string, int>> newStates;
                    if (calculateNewRoute(newStates)) {
                        setNewMachineState(newStates, 0.0);
                    } else {
                        throw(std::invalid_argument("imposible to route"));
                    }
                } else {
                    throw(std::invalid_argument("there is not a flow between "  +  std::to_string(idSource) + "->" + std::to_string(idTarget)));
                }
            } else {
                throw(std::invalid_argument("some container is empty"));
            }
        } else {
            throw(std::invalid_argument(std::to_string(idTarget) + ", does not exists as a container"));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idSource) + ", does not exists as a container"));
    }
}

void FluidicMachineModel::setTubeFlow(int edgeIdSource, int edgeIdTarget, double flowRate) {

}

void FluidicMachineModel::setTranslationStack(std::shared_ptr<TranslationStack> translationStack) {

}

void FluidicMachineModel::updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory) {

}
