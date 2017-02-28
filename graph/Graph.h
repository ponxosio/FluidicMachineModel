/*
 * Graph.h
 *
 *  Created on: 7 de mar. de 2016
 *      Author: angel
 */

#ifndef SRC_GRAPH_GRAPH_H_
#define SRC_GRAPH_GRAPH_H_

//INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <stdexcept>
#include <type_traits>

// file management
#include <iostream>
#include <sstream>
#include <fstream>

// data structures
#include <memory>
#include <vector>
#include <queue>
#include <utility>
#include <unordered_map>
#include <tuple>

#include "util/Utils.h"
#include "Edge.h"
#include "Node.h"

//cereal
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/polymorphic.hpp>

/**
 *
 */
template <class NodeType, class EdgeType> class Graph {

	// Restrict this template for using only types derived from Node and edge Interfaces
    static_assert(std::is_base_of<Node, NodeType>::value, "NodeType must extend Node");
    static_assert(std::is_base_of<Edge, EdgeType>::value, "EdgeType must extend Edge");

    typedef Node* NodePtr;
    typedef Edge* EdgePtr;

public:
	//types defs
	typedef std::shared_ptr<NodeType> NodeTypePtr;
	typedef std::shared_ptr<EdgeType> EdgeTypePtr;

	typedef vector<EdgeTypePtr> EdgeVector;
	typedef vector<NodeTypePtr> NodeVector;

	typedef std::shared_ptr<EdgeVector> EdgeVectorPtr;
	typedef std::shared_ptr<NodeVector> NodeVectorPtr;

	typedef unordered_map<int, NodeTypePtr> NodeMap;
	typedef unordered_map<int, EdgeVectorPtr> EdgeMap;
    typedef unordered_map<int, EdgeTypePtr> UniqueEdgeMap;

	typedef std::shared_ptr<NodeMap> NodeMapPtr;
	typedef std::shared_ptr<EdgeMap> EdgeMapPtr;
    typedef std::shared_ptr<UniqueEdgeMap> UniqueEdgeMapPtr;
	//

	Graph();
    Graph(const Graph & g);
	virtual ~Graph();

	//insert graph
	bool addNode(NodeTypePtr node);
    void addEdge(EdgeTypePtr edge);

	//retrieve
	NodeTypePtr getNode(int containerId);
    EdgeTypePtr getEdge(int idSource, int idTarget);
	const EdgeVectorPtr getLeavingEdges(int idSource);
	const EdgeVectorPtr getArrivingEdges(int idSource);
	bool areConnected(int idSource, int idTarget);

	//delete from graph
	bool removeNode(int nodeID);
    void removeEdge(EdgeType & edge);
	void clear();

	//save graph
    bool saveGraph(const string& filename) throw (std::runtime_error);
    string toString();

    inline const UniqueEdgeMapPtr getAllEdges() {
        return edgeMap;
    }

    inline const NodeMapPtr getAllNodes() {
        return nodeMap;
    }

	//SERIALIZATIoN
	template<class Archive>
	void serialize(Archive & ar) {
        ar(nodeMap, leavingEdges, arrivingEdges, edgeMap);
	}
protected:
	//attributes
	NodeMapPtr nodeMap;
	EdgeMapPtr leavingEdges;
	EdgeMapPtr arrivingEdges;
    UniqueEdgeMapPtr edgeMap;

	// cretors
	NodeVectorPtr makeNodeVector();
	EdgeVectorPtr makeEdgeVector();
	NodeMapPtr makeNodeMap();
	EdgeMapPtr makeEdgeMap();
};

template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::Graph() {
	nodeMap = makeNodeMap();
	leavingEdges = makeEdgeMap();
	arrivingEdges = makeEdgeMap();
    edgeMap = std::make_shared<UniqueEdgeMap>();
}

template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::Graph(const Graph & g) {
    nodeMap = makeNodeMap();
    leavingEdges = makeEdgeMap();
    arrivingEdges = makeEdgeMap();
    edgeMap = std::make_shared<UniqueEdgeMap>();

    for(auto pair: *g.nodeMap.get()) {
        NodeTypePtr ptr = pair.second;
        NodeTypePtr ptrCopy((NodeType*)ptr->clone());
        addNode(ptrCopy);
    }

    for(auto pair: *g.edgeMap.get()) {
        EdgeTypePtr ptr = pair.second;
        EdgeTypePtr ptrCopy((EdgeType*)ptr->clone());
        addEdge(ptrCopy);
    }
}

template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::~Graph() {
	clear();
}

/**
 * Deallocates all the nodes and edges, wipe out the containers without deallocating them.
 */
template <class NodeType, class EdgeType>
void Graph<NodeType,EdgeType>::clear() {
    edgeMap->clear();
	nodeMap->clear();
	leavingEdges->clear();
	arrivingEdges->clear();
}

/**
 * Adds a new node to the graph if not other  node exists with the same id
 *
 * @param node pointer to the node to add
 * @return true is the operation was successful, false otherwise
 */
template <class NodeType, class EdgeType>
bool Graph<NodeType,EdgeType>::addNode(NodeTypePtr node) {
	bool vuelta = false;
	if (nodeMap->find(node->getContainerId()) == nodeMap->end()) {
		nodeMap->insert(make_pair(node->getContainerId(), node));
		leavingEdges->insert(make_pair(node->getContainerId(), makeEdgeVector()));
		arrivingEdges->insert(make_pair(node->getContainerId(), makeEdgeVector()));
		vuelta = true;
	}
	return vuelta;
}

/**
 * Adds a new edge to the graph, does not checked for errors
 *
 * @param edge pointer to edge to insert
 */
template <class NodeType, class EdgeType>
void Graph<NodeType,EdgeType>::addEdge(EdgeTypePtr edge) {
    edgeMap->insert(std::make_pair(Utils::cantorParingFunction(edge->getIdSource(), edge->getIdTarget()),edge));
    auto leavingIt = leavingEdges->find(edge->getIdSource());
    if (leavingIt != leavingEdges->end()) {
        EdgeVectorPtr vectorLeaving = leavingIt->second;
        vectorLeaving->push_back(edge);
    }

    auto arrivingIt = arrivingEdges->find(edge->getIdTarget());
    if (arrivingIt != arrivingEdges->end()) {
        EdgeVectorPtr vectorArriving = arrivingIt->second;
        vectorArriving->push_back(edge);
    }
}

/**
 * Returns the node with the corresponding containerId
 *
 * @param containerId ID of the node
 * @return a pointer to the node if exists, NULL otherwise
 */
template <class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::NodeTypePtr Graph<NodeType,EdgeType>::getNode(int containerId) {
	NodeTypePtr vuelta = NULL;

	auto nodeContainer = nodeMap->find(containerId);
	if (nodeContainer != nodeMap->end()) {
		vuelta = nodeContainer->second;
	}
	return vuelta;
}

/**
 * Returns the first edge that joins the nodes
 *
 * @param idSource node id wich the edge points out
 * @param idTarget node id wich the edge points to
 *
 * @return a pointer to the edge object if exists, NULL otherwise
 */
template <class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::EdgeTypePtr Graph<NodeType,EdgeType>::getEdge(int idSource, int idTarget) {
        EdgeTypePtr edge = NULL;
        auto it = edgeMap->find(Utils::cantorParingFunction(idSource, idTarget));
        if (it != edgeMap->end()) {
            return it->second;
        }
        return edge;
}

/**
 * Returns all the neighbors of the node with idSource
 *
 * @param idSource ID of the node to get the neighbors from
 * @return a pointer to a vector with all the edges pointing out the node with idSource if exists, NULL otherwise
 *
 */
template <class NodeType, class EdgeType>
const typename Graph<NodeType, EdgeType>::EdgeVectorPtr Graph<NodeType,EdgeType>::getLeavingEdges(int idSource) {
	EdgeVectorPtr vuelta = NULL;

	auto it = leavingEdges->find(idSource);
	if (it != leavingEdges->end()) {
		vuelta = it->second;
	}
	return vuelta;
}

/**
 * Removes and deallocates a node and all the edges connected to it from the graph. If the idNode exits.
 * @param idNode id of the node to remove
 * @return true if the idNode exists, false in other case.
 */
template <class NodeType, class EdgeType>
bool Graph<NodeType,EdgeType>::removeNode(int nodeID) {
	bool vuelta = false;

	auto nodeRemove = nodeMap->find(nodeID);
	if (nodeRemove != nodeMap->end()) {
		//removes the node
		//delete nodeRemove->second;
		nodeMap->erase(nodeID);

		//remove all the edges that use idNode
        for(auto it = edgeMap->begin(); it != edgeMap->end(); ) {
            EdgeTypePtr actual = it->second;
            if ((actual->getIdSource() == nodeID) ||
                (actual->getIdTarget() == nodeID))
            {
                it = edgeMap->erase(it);
            } else {
                ++it;
            }
        }

		//remove neighbor from the list
		leavingEdges->erase(nodeID);
		arrivingEdges->erase(nodeID);

		vuelta = true;
	}
	return vuelta;
}

/**
 * Removes and deallocates all edges from the list that are equals as the passing one
 * @param edge object to be compared for removing from the vector
 */
template<class NodeType, class EdgeType>
void Graph<NodeType, EdgeType>::removeEdge(EdgeType & edge) {
    auto it_map = edgeMap->find(Utils::cantorParingFunction(edge.getIdSource(), edge.getIdTarget()));
    if (it_map != edgeMap->end()) {
        edgeMap->erase(it_map);
    }

    auto it_lea = leavingEdges->find(edge.getIdSource());
    if (it_lea != leavingEdges->end()) {
        EdgeVectorPtr edges = it_lea->second;
        for(auto it = edges->begin(); it != edges->end();) {
            EdgeTypePtr actual = *it;
            if (actual->getIdTarget() == edge.getIdTarget()) {
                it = edges->erase(it);
            } else {
                ++it;
            }
        }
    }

    auto it_arr = arrivingEdges->find(edge.getIdSource());
    if (it_arr != arrivingEdges->end()) {
        EdgeVectorPtr edges = it_arr->second;
        for(auto it = edges->begin(); it != edges->end();) {
            EdgeTypePtr actual = *it;
            if (actual->getIdTarget() == edge.getIdTarget()) {
                it = edges->erase(it);
            } else {
                ++it;
            }
        }
    }
}

/**
 * Save the graph as a text file compatible with GraphViz. with the passing filename.
 *
 * @param filename name of the file where the graph if saved
 * @return true if the operation was successful , false otherwise
 */
template <class NodeType, class EdgeType>
bool Graph<NodeType,EdgeType>::saveGraph(const string& filename) throw (std::runtime_error) {
	bool error = false;
	ofstream myfile(filename.c_str());

	if (myfile.is_open()) {
		myfile << "digraph G {" << endl;

		// writing the nodes...
		for (auto it = nodeMap->begin();
				it != nodeMap->end(); ++it) {
			NodeTypePtr actual = it->second;
			myfile << actual->toText()<< endl;
		}

		// writing the edges...
        for (auto it = edgeMap->begin(); it != edgeMap->end(); ++it) {
            EdgeTypePtr actual = it->second;
			myfile << actual->toText() << endl;
		}
		myfile << "}";
		myfile.close();
	} else {
        throw (std::runtime_error("Graph::saveGraph-> unable to open file: " + filename));
		error = true;
	}
	return !error;
}


template<class NodeType, class EdgeType>
const typename Graph<NodeType, EdgeType>::EdgeVectorPtr Graph<NodeType, EdgeType>::getArrivingEdges(
		int idSource) {
	EdgeVectorPtr vuelta = NULL;

	auto it = arrivingEdges->find(idSource);
	if (it != arrivingEdges->end()) {
		vuelta = it->second;
	}
	return vuelta;
}

/**
 * Search all disjoint subgraphs, the idea is to color connect nodes with the same color:
 *
 * for all edges->
 * 	*) if both connected nodes has no color: set the same random color to both;
 * 	*) if only one has a color: set the empty node's color so they be the same.
 * 	*) if both has one color and those colors are different: update all the nodes with
 * 		the bigger color to match the smaller one. (merge)
 */
/*template<class NodeType, class EdgeType>
void Graph<NodeType, EdgeType>::calculateSubgraphs() {
	subGraphs = makeSubGraph();
	unordered_map<int,int> node_colorMap;
	int lastColor = 0;

	for (auto it = edgeList->begin(); it != edgeList->end(); ++it) {
		EdgeTypePtr actual = *it;
		int idSource = actual->getIdSource();
		int idTarget = actual->getIdTarget();

		int colorSource =
				(node_colorMap.find(idSource) != node_colorMap.end()) ?
						node_colorMap.find(idSource)->second : -1;
		int colorTarget =
				(node_colorMap.find(idTarget) != node_colorMap.end()) ?
						node_colorMap.find(idTarget)->second : -1;

		if ((colorSource == -1) && (colorTarget == -1)) {
			node_colorMap.insert(make_pair(idTarget,lastColor));
			node_colorMap.insert(make_pair(idSource,lastColor));
			lastColor++;
		} else if ((colorSource != -1) && (colorTarget == -1)) {
			node_colorMap.insert(make_pair(idTarget,colorSource));
		} else if ((colorSource == -1) && (colorTarget != -1)) {
			node_colorMap.insert(make_pair(idSource,colorTarget));
		} else if ((colorSource != -1) && (colorTarget != -1)) {
            // If the two has color a merge must be performed,
             // the bigger color will be change to the smaller one
			if (colorSource != colorTarget) {
				int colorWin = min(colorSource, colorTarget);
				int colorChange =
						(colorWin == colorSource) ? colorTarget : colorSource;
				for (auto it = node_colorMap.begin(); it != node_colorMap.end();
						++it) {
					if (it->second == colorChange) {
						it->second = colorWin;
					}
				}
			}
		}
	}
	unordered_map<int,SubGraphElem> temp_color_nodeMap;
	for (auto it = node_colorMap.begin(); it != node_colorMap.end(); ++it) {
		int color = it->second;
		int idNode = it->first;

		auto nodeList = temp_color_nodeMap.find(color);
		if (nodeList == temp_color_nodeMap.end()) { //new color
			SubGraphElem newPair = make_pair(makeNodeVector(), makeEdgeVector());
			get<0>(newPair)->push_back(getNode(idNode));
			get<1>(newPair)->insert(get<1>(newPair)->end(), getLeavingEdges(idNode)->begin(), getLeavingEdges(idNode)->end());

			temp_color_nodeMap.insert(make_pair(color, newPair));
		} else {
			SubGraphElem actualPair = nodeList->second;
			get<0>(actualPair)->push_back(getNode(idNode));
			get<1>(actualPair)->insert(get<1>(actualPair)->end(), getLeavingEdges(idNode)->begin(), getLeavingEdges(idNode)->end());
		}
	}

	for (auto it = temp_color_nodeMap.begin(); it != temp_color_nodeMap.end(); ++it) {
		subGraphs->push_back(it->second);
	}
}*/

template<class NodeType, class EdgeType>
string Graph<NodeType, EdgeType>::toString() {
	ostringstream myfile;
	myfile << "digraph{";

	//print the nodes
	for (auto it =
			nodeMap->begin(); it != nodeMap->end(); ++it) {
		NodeTypePtr actual = it->second;
		myfile << actual->toText();
	}

	//print the edges
    for (auto it = edgeMap->begin();it != edgeMap->end(); ++it) {
        EdgeTypePtr actual = it->second;
		myfile << actual->toText();
	}

	myfile << "}";
	return myfile.str();
}

template<class NodeType, class EdgeType>
bool Graph<NodeType, EdgeType>::areConnected(int idSource, int idTarget) {
    return (edgeMap->find(Utils::cantorParingFunction(idSource, idTarget)) != edgeMap->end());
}

template<class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::NodeVectorPtr Graph<NodeType, EdgeType>::makeNodeVector() {
	return std::make_shared<NodeVector>();
}

template<class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::EdgeVectorPtr Graph<NodeType, EdgeType>::makeEdgeVector() {
	return std::make_shared<EdgeVector>();
}

template<class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::NodeMapPtr Graph<NodeType, EdgeType>::makeNodeMap() {
	return std::make_shared<NodeMap>();
}

template<class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::EdgeMapPtr Graph<NodeType, EdgeType>::makeEdgeMap() {
	return std::make_shared<EdgeMap>();
}

#endif /* SRC_GRAPH_GRAPH_H_ */
