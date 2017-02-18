#ifndef MACHINEGRAPHGENERATOR_H
#define MACHINEGRAPHGENERATOR_H

#include <vector>
#include <memory>
#include <sstream>
#include <numeric>

#include "machinegraph.h"
#include "fluidicedge/tubeedge.h"
#include "fluidicnode/fluidicmachinenode.h"
#include "fluidicnode/valvenode.h"
#include "util/sequence.h"

#include "fluidicmachinemodel_global.h"

class MACHINEGRAPHGENERATOR_EXPORT MachineGraphGenerator
{
    typedef struct AbsuluteValueSum_ {
        int operator()(int x, int y) {return std::abs(x) + std::abs(y);}
    }AbsuluteValueSum;

public:
    MachineGraphGenerator(std::shared_ptr<MachineGraph> graph_original);
    virtual ~MachineGraphGenerator();

    bool calculateNext();
    std::string getStateStr();

    inline const std::vector<std::tuple<int,int>> & getActualStateVector() {
        return actualStateGeneratedVector;
    }
    inline const std::shared_ptr<MachineGraph> & getActualGraphGenerated() {
        return actualGraphGenerated;
    }
    inline int getMaxPumpsIndex() {
        return pumpsMaxIndex;
    }

protected:
    int pumpsMaxIndex;
    std::shared_ptr<MachineGraph> graph_original;

    int maxSequenceIndex;
    std::vector<std::tuple<int,Sequence*>> statesSequenceVector;

    std::shared_ptr<MachineGraph> actualGraphGenerated;
    std::vector<std::tuple<int,int>> actualStateGeneratedVector;

    void advanceSequence();
    std::vector<short int> getNextStateFromSequence();

    bool isValidSequence(const std::vector<short int> & sequence);

    void fillActualStateGeneratedVector(const std::vector<short int> & actualState);

    bool isValidGraph(const std::shared_ptr<MachineGraph> & graph_original);
    bool canOpenContainersConnect(const std::shared_ptr<MachineGraph> & graph_original);
    bool isValidConnectedComponent(const std::shared_ptr<MachineGraph> & graph_original);
    void countLeavingArringEdgesVector(const MachineGraph::GraphType::EdgeVectorPtr & tubesToCheck,
                                       bool arriving,
                                       int & numArriving,
                                       int & numLeaving);

    std::shared_ptr<MachineGraph> createGraphForState(const std::vector<short int> & estates) throw(std::invalid_argument);
    void setValve(int valveId, short int state, std::shared_ptr<MachineGraph> graph_original);

    bool setPump(int pumpId, short int state, std::shared_ptr<MachineGraph> graph_original) throw(std::invalid_argument);
    bool setTubesDirectionFromPump(int pumpId, TubeEdge::TubeDirection dir, std::shared_ptr<MachineGraph> graph_original) throw(std::invalid_argument);

    bool processTubeVector(const MachineGraph::GraphType::EdgeVector & tubesToCheck,
                           TubeEdge::TubeDirection dir,
                           int nodeProcesing,
                           std::shared_ptr<MachineGraph> graph_original,
                           const std::unordered_set<int> & nodesChecked,
                           std::vector<int> & nodesToCheck);

    bool processMultipathValve(int valveNode,
                               int enteringNode,
                               TubeEdge::TubeDirection dir,
                               std::shared_ptr<MachineGraph> graph,
                               const std::unordered_set<int> & nodesChecked,
                               std::vector<int> & nodesToCheck);
};

#endif // MACHINEGRAPHGENERATOR_H
