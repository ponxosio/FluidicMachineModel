#ifndef MACHINEFLOW_H
#define MACHINEFLOW_H

#include <unordered_map>
#include <vector>

#include "util/Utils.h"

class MachineFlow
{
    typedef std::unordered_map<std::tuple<short int, short int>, FlowsList, > FlowMap;
    typedef std::vector<short int> ContainersList;
    typedef std::vector<std::tuple<short int, short int, bool>> TubesList;

public:
    typedef std::vector<std::tuple<ContainersList,TubesList>> FlowsList;

    MachineFlow();
    virtual ~MachineFlow();

    bool hasMoreFlows(short int openContainerId);
    void addNewFlow(short int openContainerIdStart,
                    short int openContainerIdEnd,
                    const std::vector<short int> * intermediateContainers = NULL,
                    const std::vector<std::tuple<short int, short int, bool>> * intermediateTubes = NULL);

    FlowsList removeNewFlow(short int openContainerIdStart,
                    short int openContainerIdEnd,
                    const std::vector<short int> * intermediateContainers = NULL,
                    const std::vector<std::tuple<short int, short int, bool>> * intermediateTubes = NULL);

protected:
    FlowMap flowMap;
    std::unordered_map<short int, short int> openContainers;


};

#endif // MACHINEFLOW_H
