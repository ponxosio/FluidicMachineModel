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

public:

	//types defs
	typedef Node* NodePtr;
	typedef Edge* EdgePtr;

	typedef std::shared_ptr<NodeType> NodeTypePtr;
	typedef std::shared_ptr<EdgeType> EdgeTypePtr;

	typedef vector<EdgeTypePtr> EdgeVector;
	typedef vector<NodeTypePtr> NodeVector;

	typedef std::shared_ptr<EdgeVector> EdgeVectorPtr;
	typedef std::shared_ptr<NodeVector> NodeVectorPtr;

	typedef unordered_map<int, NodeTypePtr> NodeMap;
	typedef unordered_map<int, EdgeVectorPtr> EdgeMap;

	typedef std::shared_ptr<NodeMap> NodeMapPtr;
	typedef std::shared_ptr<EdgeMap> EdgeMapPtr;

	typedef std::tuple<NodeVectorPtr, EdgeVectorPtr> SubGraphElem;
	typedef vector<SubGraphElem> SubGraph;
	typedef std::shared_ptr<SubGraph> SubGraphPtr;
	//

	Graph();
	virtual ~Graph();

	//insert graph
	bool addNode(NodeTypePtr node);
	bool addEdge(EdgeTypePtr edge);

	//retrieve
	NodeTypePtr getNode(int containerId);
	const EdgeVectorPtr getLeavingEdges(int idSource);
	const EdgeVectorPtr getArrivingEdges(int idSource);
	const NodeVectorPtr getAllNodes();
	bool areConnected(int idSource, int idTarget);

	//delete from graph
	bool removeNode(int nodeID);
	void removeEdge(const EdgeType & edge);
	void clear();

	//save graph
    bool saveGraph(const string& filename) throw (std::runtime_error);

	//getters
	inline EdgeVectorPtr getEdgeList() {
		return edgeList;
	}
	inline SubGraphPtr getSubGraphs() {
		if (subGraphs == NULL) {
			calculateSubgraphs();
		}
		return subGraphs;
	}

	string toString();

	//SERIALIZATIoN
	template<class Archive>
	void serialize(Archive & ar) {
		ar(nodeMap, leavingEdges, arrivingEdges, subGraphs, edgeList);
	}
protected:
	//attributes
	NodeMapPtr nodeMap;
	EdgeMapPtr leavingEdges;
	EdgeMapPtr arrivingEdges;
	SubGraphPtr subGraphs;
	EdgeVectorPtr edgeList;

	//methods
	void calculateSubgraphs();

	// cretors
	NodeVectorPtr makeNodeVector();
	EdgeVectorPtr makeEdgeVector();
	NodeMapPtr makeNodeMap();
	EdgeMapPtr makeEdgeMap();
	SubGraphPtr makeSubGraph();
};

template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::Graph() {
	nodeMap = makeNodeMap();
	leavingEdges = makeEdgeMap();
	arrivingEdges = makeEdgeMap();
	subGraphs = NULL;
	edgeList = makeEdgeVector();
}

template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::~Graph() {
	clear();
	//delete edgeList;
	//delete nodeMap;
	//delete leavingEdges;
	//delete arrivingEdges;
	/*if (subGraphs != NULL) {
		delete subGraphs;
	}*/
}

/**
 * Deallocates all the nodes and edges, wipe out the containers without deallocating them.
 */
template <class NodeType, class EdgeType>
void Graph<NodeType,EdgeType>::clear() {
	// free the edges
	/*for (auto it = edgeList->begin(); it != edgeList->end();
			++it) {
		delete *it;
	}*/
	edgeList->clear();

	//free the nodes
	/*for (auto it = nodeMap->begin();
			it != nodeMap->end(); ++it) {
		delete it->second;
	}*/
	nodeMap->clear();

	//free the neighbor map
	/*for (auto it = leavingEdges->begin();
			it != leavingEdges->end(); ++it) {
		delete it->second;
	}*/
	leavingEdges->clear();
	/*for (auto it =
			arrivingEdges->begin(); it != arrivingEdges->end(); ++it) {
		delete it->second;
	}*/
	arrivingEdges->clear();

	if (subGraphs != NULL) {
		//free the neighbor map
		/*for (auto it =
				subGraphs->begin(); it != subGraphs->end(); ++it) {
			SubGraphElem actual = *it;
			delete actual.second;
			delete actual.first;
		}*/
		subGraphs->clear();
	}
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
 * Adds a new edge to the graph if the nodes that the edge connects does not exists and an equal(same attributes) edge
 * is not already in the graph
 *
 * @param edge pointer to edge to insert
 * @return true if successful, false otherwise
 */
template <class NodeType, class EdgeType>
bool Graph<NodeType,EdgeType>::addEdge(EdgeTypePtr edge) {
	bool vuelta = false;

	auto nodeSource = nodeMap->find(edge->getIdSource());
	// if the two node that the edge connects exits
	if ((nodeSource != nodeMap->end())
			&& (nodeMap->find(edge->getIdTarget()) != nodeMap->end())) {
		edgeList->push_back(edge);
		EdgeVectorPtr vectorLeaving = leavingEdges->find(edge->getIdSource())->second;
		vectorLeaving->push_back(edge);
		EdgeVectorPtr vectorArriving = arrivingEdges->find(edge->getIdTarget())->second;
		vectorArriving->push_back(edge);
		vuelta = true;
	}
	return vuelta;
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
template <class NodeType, class EdgeType>
const typename Graph<NodeType, EdgeType>::NodeVectorPtr Graph<NodeType,EdgeType>::getAllNodes() {
	NodeVectorPtr vuelta = makeNodeVector();

	for (auto it = nodeMap->begin();
				it != nodeMap->end(); ++it) {
		vuelta->push_back(it->second);
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
		auto it = edgeList->begin();
		while (it != edgeList->end()) {
			EdgeTypePtr actual = *it;
			if ((actual->getIdSource() == nodeID)
					|| (actual->getIdTarget() == nodeID)) {
                it = edgeList->erase(it);
				//delete actual;
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
void Graph<NodeType, EdgeType>::removeEdge(const EdgeType & edge) {
	auto it = edgeList->begin();
	while (it != edgeList->end()) {
		EdgeTypePtr actual = *it;
		if (actual->equals(edge)) {
            it = edgeList->erase(it);
			//delete actual;
		} else {
			++it;
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
		for (auto it = edgeList->begin();
				it != edgeList->end(); ++it) {
			EdgeTypePtr actual = *it;
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
template<class NodeType, class EdgeType>
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
			/* If the two has color a merge must be performed,
			 * the bigger color will be change to the smaller one
			 */
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
}

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
	for (auto it = edgeList->begin();
			it != edgeList->end(); ++it) {
		EdgeTypePtr actual = *it;
		myfile << actual->toText();
	}

	myfile << "}";
	return myfile.str();
}

template<class NodeType, class EdgeType>
bool Graph<NodeType, EdgeType>::areConnected(int idSource, int idTarget) {
	bool vuelta = false;
	const EdgeVectorPtr neighbors = getLeavingEdges(idSource);

	auto it = neighbors->begin();
	while (!vuelta && (it != neighbors->end())) {
		EdgeTypePtr cast = *it;
		vuelta = (cast->getIdTarget() == idTarget);
		++it;
	}
	return vuelta;
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

template<class NodeType, class EdgeType>
typename Graph<NodeType, EdgeType>::SubGraphPtr Graph<NodeType, EdgeType>::makeSubGraph() {
	return std::make_shared<SubGraph>();
}

#endif /* SRC_GRAPH_GRAPH_H_ */
