#ifndef FLUIDICMACHINEMODEL_H
#define FLUIDICMACHINEMODEL_H

#define UNDEFINE_VAR_STATUS -1

#include <bitset>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <commonmodel/modelinterface/modelinterface.h>
#include <commonmodel/plugininterface/pluginabstractfactory.h>
#include <protocolGraph/ProtocolGraph.h>

#include <utils/units.h>
#include "utils/machineflow.h"

#include "fluidicmachinemodel/machinegraph.h"
#include "fluidicmachinemodel/machine_graph_utils/graphrulesgenerator.h"
#include "fluidicmachinemodel/machine_graph_utils/machinestate.h"
#include "fluidicmachinemodel/machine_graph_utils/variablenominator.h"

#include "fluidicmachinemodel/constraintssolverinterface/translationstack.h"
#include "fluidicmachinemodel/constraintssolverinterface/routingengine.h"

#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"
#include "fluidicmachinemodel/fluidicnode/containernode.h"
#include "fluidicmachinemodel/fluidicnode/pumpnode.h"
#include "fluidicmachinemodel/fluidicnode/valvenode.h"
#include "fluidicmachinemodel/fluidicedge/tubeedge.h"

#include "fluidicmachinemodel/rules/rule.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

/**
 * @brief The FluidicMachineModel class allows doing liquid movements in a MachineGraph
 *
 * This class is the main interface of the library. Allows to set/unset flows over a machine and communicate with a MachineGraph
 * components to carry out. Via TranslationStack communicates with the constraints engine and via the PluginAbstractFactory
 * communicates with the physical actuators of the machine. The main wrokflow of this class is setting a set of flow over the machine,
 * calculate what state must the pumps and valves of the system have in order to maintain these flows and communicate with the physical
 * components of the machine to change this states.
 *
 * @sa TranslationStack, PluginAbstractFactory, ModelInterface
 */
class FLUIDICMACHINEMODELSHARED_EXPORT FluidicMachineModel : public ModelInterface
{   
public:
    /**
     * @brief FluidicMachineModel creates a new FluidicMachineModel.
     *
     * FluidicMachineModel creates a new FluidicMachineModel. The precision numbers are neccesary to convert rates from real number
     * to integer because the contrainst engine works faster with these kind of numbers. An overflow_error exception is throw if
     * the maximun number of open conatiner, this is calculated with the ratePrecisionInteger and the ratePrecision, is surpass
     *
     * @param graph object with the machine layout where the liquid movents are going to be carry out.
     * @param translationStack object that implements the interface TranslationStack, connected with the
     * constraints engine the class that translated and use the logical rules that governs how to set the flows.
     * @param ratePrecisionInteger number that set the maximun digits allowed for specifying a rate, for example
     * if this number is 3 the rate values must be between 0 and 999.
     * @param ratePrecisionDecimal number that set the decimal precision of a rate, for example if this number is
     * 2 the system will truncate rates to 2 decimals precision 2.339 will be change to 2.34.
     *
     * @sa MachineGraph, maxOpenContainers
     */
    FluidicMachineModel(std::shared_ptr<MachineGraph> graph,
                        std::shared_ptr<TranslationStack> translationStack = std::shared_ptr<TranslationStack>(),
                        short int ratePrecisionInteger = 3,
                        short int ratePrecisionDecimal = 2,
                        double defaultRate = 0,
                        units::Volumetric_Flow defaultRateUnits = (units::ml / units::hr)) throw(std::overflow_error);
    /**
     * @brief ~FluidicMachineModel empty destructor
     */
    virtual ~FluidicMachineModel();

    /**
     * @brief canDoMovement returns true for 'continuous' and 'discrete' movement
     *
     * canDoMovement returns true if the mask contains the flags 'continuous', 'discrete'
     * or both, false otherwise.
     *
     * @param mask mask with the movements flags to check.
     * @return true if the movements flags are 'continuos', 'discrete' or both.
     *
     * @sa ModelInterface
     */
    virtual bool canDoMovement(unsigned long mask);

    /**
     * @brief getComponent returns a particular node as a ComponentInterface
     *
     * getComponent returns the node with the corresponding ID as a ComponentInterface.
     *
     * @param virtualContainer name of the virtual container of the protocol.
     * @return a pointer to the corresponding component.
     *
     * @exception an invalid_argument exception is thrown if the id does not match any node id in the graph.
     */
    virtual std::shared_ptr<ComponentInterface> getComponent(int virtualContainer) throw(std::invalid_argument);

    /**
     * @brief loadContainer specifies that a container is loaded with a liquid.
     *
     * loadContainer specifies that a container is loaded with a certain amunt of liquid,
     * if a container is not loaded no liquid can be extracted from it.
     *
     * @param id of the node of the graph that will be loaded, the the specify id must be
     * a container node.
     * @param volume loaded in the container.
     */
    virtual void loadContainer(int id, units::Volume volume) throw(std::invalid_argument);

    virtual units::Time transferLiquid(int sourceId, int targetId,  units::Volume volume) throw(std::invalid_argument);
    virtual void stopTransferLiquid(int sourceId, int targetId) throw(std::invalid_argument);

    virtual units::Time mix(int sourceId1, int sourceId2, int targetId,  units::Volume volume1, units::Volume volume2) throw(std::invalid_argument);
    virtual void stopMix(int sourceId1, int sourceId2, int targetId) throw(std::invalid_argument);
    /**
     * @brief setContinuousFlow set a new flow betwen two nodes.
     *
     * this method does not send a new query to the constraints engine nither send any command
     * to the devices of the machine to do so calculateNewRoute() is used after setting one or
     * several flows.
     *
     * @param idStart id of the node to extract liquid from
     * @param idEnd id of the node to inject liquid to
     * @param flowRate rate at wich the liquid is moved, needs to be compatible
     * with the values ratePrecisionInteger, ratePrecisionDecimal.
     *
     * @sa calculateNewRoute(), ratePrecisionInteger, ratePrecisionDecimal, MachineFlow
     */
    virtual void setContinuousFlow(int idStart, int idEnd, units::Volumetric_Flow flowRate);
    /**
     * @brief setContinuousFlow set a new flow betwen two nodes.
     *
     * this method does not send a new query to the constraints engine nither send any command
     * to the devices of the machine to do so calculateNewRoute() is used after setting one or
     * several flows.
     *
     * @param containersIds list of the containers were liquid is being tranfer, from the frist container
     * the liquid is extracted, passes by the containers in between and is injected in the last container,
     * the first and last containers must be open containers.
     * @param flowRate rate at wich the liquid is moved, needs to be compatible
     * with the values ratePrecisionInteger, ratePrecisionDecimal.
     *
     * @sa calculateNewRoute(), ratePrecisionInteger, ratePrecisionDecimal, MachineFlow
     */
    virtual void setContinuousFlow(const std::vector<int> & containersIds, units::Volumetric_Flow flowRate) throw(std::invalid_argument);

    /**
     * @brief stopContinuousFlow stop all flows that starts and ends with the corrsponding nodes.
     *
     * Stop all flow/s set in the machine that starts and ends with the corresponding nodes, does not
     * matter theirs flow's rates, for example if these two flows are sets on a machine: 1->2->4 and 1->3->4
     * calling stopContinuosFlow(1,4) will remove both flows.
     *
     * This method does not send a new query to the constraints engine nither
     * send any command to the devices of the machine to do so calculateNewRoute() is used after setting one
     * or several flows.
     *
     * @param idStart id of the node the flow/s extracts liquid from
     * @param idEnd id of the node the flow/s injects liquid to
     *
     * @sa calculateNewRoute(), MachineFlow
     */
    virtual void stopContinuousFlow(int idStart, int idEnd);

    /**
     * @brief stopContinuousFlow stop all flows that starts and ends with the corrsponding nodes.
     *
     * Stop all flow/s set in the machine that match with the corresponding nodes, does not
     * matter theirs flow's rates, for example if these two flows are sets on a machine: 1->2->4 and 1->3->4
     * calling stopContinuosFlow([1,3,4]) will remove only 1->3->4 flow.
     *
     * This method does not send a new query to the constraints engine nither
     * send any command to the devices of the machine to do so calculateNewRoute() is used after setting one
     * or several flows.
     *
     * @param containersIds list of the containers that defines the flow to be removed, from the frist container
     * the liquid is extracted, passes by the containers in between and is injected in the last container,
     * the first and last containers must be open containers.
     *
     * @sa calculateNewRoute(), MachineFlow
     */
    virtual void stopContinuousFlow(const std::vector<int> & containersIds) throw(std::invalid_argument);

    /**
     * @brief processFlows sends a new query to the constraint engine and send comminucates the changes to the pyshical devices.
     *
     * Sends a new query to the constraint engine with the actuals flows in the machine set or stop using setContinuosFlow and
     * stopContinuos flow methods. The constraints engine returns the new states the valves and pumps must be in order to mantain
     * those flows in the machine, these new states are transmitted to the pyshical devices via the plugin system. This methods also
     * createas a new RoutingEngine translating the rules of the machine if neccesary.
     *
     * @sa RoutingEngine, rules
     */
    virtual void processFlows() throw(std::runtime_error);

    bool checkFlows(const MachineFlow::FlowsVector & flows2Set) throw(std::runtime_error);

    /**
     * @brief setTranslationStack sets the TransaltionStack of the system, this is the object that interfaces with the constraints engine.
     *
     * setTranslationStack changes the TranslationStack of these machine, this object implements the TranslatinStack interface and
     * allows communication with the constraint engine. This object translate the abstract rules of extracted from the graph to
     * specific rules that the constraint engine understands, also is capable of creating a new RoutingEngine that is the object
     * capable of receiving new querys to calculate the state needed to maintain several flows over the machine.
     *
     * @param translationStack object that implements the TranslationStack interface, This object translate the abstract rules of
     * extracted from the graph to specific rules that the constraint engine understands, also is capable of creating a new RoutingEngine
     * that is the object capable of receiving new querys to calculate the state needed to maintain several flows over the machine.
     *
     * @sa TrasnlationStack, RoutingEngine, graph
     */
    void setTranslationStack(std::shared_ptr<TranslationStack> translationStack);

    /**
     * @brief updatePluginFactory sets the PluginAbstractFactory of the system, this object interfaces with the physical components of the machine
     *
     * updatePluginFactory sets the PluginAbstractFactory of the system, this object implements the PluginAbstractFactory interface and allows
     * communications with the physical components of the machine. This method sets the updatePluginFactory of all the nodes of the graph attribute.
     *
     * @param factory new object that implements PluginAbstractFactory and allows communications with the physical components of the machine.
     *
     * @sa PluginAbstractFactory
     */
    virtual void updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory);

    /**
     * @brief getNode return a pointer to the node with the corresponding id.
     *
     * getNode returns a pointer to the node with the corresponding id of the graph, an invalid_argument exception is thrown if
     * no node exists with the given id at the graph atribute.
     *
     * @param id number with the id of the node to be retreve, this id must exist on the graph attribute
     * @return a pointer to the node with the unique given id.
     *
     * @sa MachineGraph, graph, FluidicMachineNode
     */
    std::shared_ptr<FluidicMachineNode> getNode(int id) throw(std::invalid_argument);

    inline void clearDisabledPumps() {
        disabledPumps.clear();
    }

    inline void disablePump(int pumpId) {
        disabledPumps.insert(pumpId);
    }

    inline void enablePump(int pumpId) {
        auto finded = disabledPumps.find(pumpId);
        if (finded != disabledPumps.end()) {
            disabledPumps.erase(finded);
        }
    }

    /**
     * @brief setDefaultRateUnits defaults units a rate will be send to the plugin system.
     *
     * setDefaultRateUnits defaults units a rate will be send to the plugin system. Any flow set
     * will be converted to this units before being sent to the constraints solver because it cannot
     * manage units. This value must be cohesive with the range of values the pumps of the machines
     * can operate so the constraint solver can use smaller numbers to avoid overflow_exceptions.
     *
     * @param defaultUnits volumetric_flow default units
     */
    inline void setDefaultRate(double defaultRate) {
        this->defaultRate = defaultRate;
    }

    inline void setDefaultRateUnits(units::Volumetric_Flow defaultRateUnits) {
        this->defaultRateUnits = defaultRateUnits;
    }

    inline double getDefaultRate() const {
        return defaultRate;
    }

    inline units::Volumetric_Flow getDefaultRateUnits() const {
        return defaultRateUnits;
    }

    inline const std::shared_ptr<const MachineGraph> getMachineGraph() const {
        return graph;
    }
protected:
    double defaultRate;
    units::Volumetric_Flow defaultRateUnits;
    /**
     * @brief maxOpenContainer indicates maximun number of open container that the graph can has.
     *
     * maxOpenContainer indicates maximun number of open container that the graph can has. This number is calculated
     * using the ratePrecisionInteger and the ratePrecisionDecimal by the class MachineState.
     *
     * @sa FluidicMachineModel(), MachineState
     */
    short int maxOpenContainer;

    /**
     * @brief graph pointer to the graph structure that represents the layout of the machine where liquid movement is done.
     *
     * @sa MachineGraph.
     */
    std::shared_ptr<MachineGraph> graph;

    /**
     * @brief actualFullMachineState the actual state of the machine.
     *
     * actualFullMachineState the actual state of the machine. This means the position of every valve, the direction and rate of
     * avery pump and the direction, liquid id and rate of the liquid passing through every tube and container.
     *
     * @sa MachineState
     */
    MachineState actualFullMachineState;
    /**
     * @brief flowEngine object that manage the actions of adding and stoping flows.
     *
     * flowEngine object that manage the actions of adding and stoping flows. When a new flow is set of stoped this object manages
     * so the order of the flow is maintained in a coherent maner despite the order the instructions arrive.
     *
     * @sa MachineFlow
     */
    MachineFlow flowEngine;
    /**
     * @brief translationStack the object that interfaces with the constraints engine.
     *
     * translationStack object that implements the TranslationStack interface, This object translate the abstract rules of
     * extracted from the graph to specific rules that the constraint engine understands, also is capable of creating a new RoutingEngine
     * that is the object capable of receiving new querys to calculate the state needed to maintain several flows over the machine.
     *
     * @sa TranslationStack
     */
    std::shared_ptr<TranslationStack> translationStack;
    /**
     * @brief rules vector with the abstract rules of the system.
     *
     * rules vector with the abstract rules of the system. This rules describe the ecuations that describe how to use the system, describe the relation
     * between tubes and containers for the diferent states of the valves and pumps of the graph. These rules are extracted from the graph machine layout.
     * These rules are translated by the TranslationStack to a format that the constraints engine(RoutingEngine) understands.
     *
     * @sa TrasnlationStack, MachineGraph, RoutingEngine, Rule, graph
     */
    std::vector<std::shared_ptr<Rule>> rules;
    /**
     * @brief routingEngine pointer to the constraint engine that calculates the machine state for a given set of flows.
     *
     * This object receives the states of the containers and the tubes and returns a complete machine state with the states
     * of the pumps and valves and teh rest of containers and tubes that were not included in the initial state.
     *
     * @sa RoutingEngine, MachineState.
     */
    std::shared_ptr<RoutingEngine> routingEngine;
    /**
     * @brief disabledPumps vector with the id of the pumps that the system cannot use.
     */
    std::unordered_set<int> disabledPumps;
    /**
     * @brief translateRules use the TranslationStack to translate the rules vector and a constructucts a new RoutingEngine.
     *
     * translateRules use the TranslationStack to translate the rules vector to a format the the constraint engine undestand,
     * also a constructucts a new RoutingEngine that will calculate the state of the machine components in order to mantain the
     * flows set in the machine.
     *
     * @return a pointer to a new constructed constarint  engine.
     *
     * @sa TrasnlationStack, Rule, RoutingEngine
     */
    std::shared_ptr<RoutingEngine> translateRules() throw(std::runtime_error);
    /**
     * @brief analizeGraph analize a graph layout and creates the rules that allows to process liquid over the machien.
     *
     * analizeGraph analize a graph layout and creates the rules, a rules are boolean conditions that defines how a machine works,
     * this logical equations are used to calculate the position of the valves and the pumps of a machine so the flows occurring over
     * the machine are met.
     *
     * @param ratePrecisionInteger number that set the maximun digits allowed for specifying a rate, for example
     * if this number is 3 the rate values must be between 0 and 999.
     * @param ratePrecisionDecimal number that set the decimal precision of a rate, for example if this number is
     * 2 the system will truncate rates to 2 decimals precision 2.339 will be change to 2.34.
     */
    void analizeGraph(short int ratePrecisionInteger, short int ratePrecisionDecimal);
    /**
     * @brief addStack2State translate a deque return by the MachineFlow object to a MachineState that the constraint engine understand
     *
     * process a deque and a rate return by the MachineFlow object to the correct values that the containers must have in the MachineState object
     * so the desired flow is calculated by the constraint engine. The MachineState is the object that the constraint engine recives in order to make
     * the calculations of what pumps and valves must be move in order to maintain a flow in the machine. An exception is throw if the flow describe in
     * the deque object does not has the first and last containers as open containers of if the rate is bigger that the maximum rate permitted.
     *
     * @param queue that represents the order the liquid is being move between containers, for example is the deque [1,2,3] is recived means
     * that liquid will be extracted from the container 1 , will pass through container 2 and be injected in container 3.
     * @param rate indicates the rate the liquid will be transfer between teh containers set in the deque
     * @param state MachineState object that will be set with the correct values so the flow described by deque and rate is processed by the
     * constraint engine.
     *
     * @sa MachineState, RoutingEngine, MachineFlow
     */
    void addStack2State(const std::deque<short int> & queue, units::Volumetric_Flow rate, MachineState & state) throw(std::invalid_argument);
    /**
     * @brief sendActualState2components send the state of the pumps and valves of the actual machine state to the physical components
     *
     * sendActualState2components communicates with the physcial components of the machine so the state of the pumps and the valves in the actualState
     * of the system is the same as the state of the pyshical components an runtime_error exception is thrown if any error occurred during this
     * communication.
     */
    void sendActualState2components() throw(std::runtime_error);

    void setUnabledPumps(MachineState & machineState);
};

#endif // FLUIDICMACHINEMODEL_H
