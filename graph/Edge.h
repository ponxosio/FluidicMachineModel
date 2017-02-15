/*
 * Edge.h
 *
 *  Created on: 7 de mar. de 2016
 *      Author: angel
 */

#ifndef SRC_GRAPH_EDGE_H_
#define SRC_GRAPH_EDGE_H_

#include <string>
#include  <stdexcept>

//cereal
#include <cereal\cereal.hpp>

/**
 * Interface with the minimum operations that all directed Edges must have
 */
class  Edge {

public:
	//All derived clases must implement this two Constructor!!
	Edge() {
		this->idSource = 0;
		this->idTarget = 0;
	}

	Edge(const Edge & edge) {
		this->idSource = edge.idSource;
		this->idTarget = edge.idTarget;
	}
	//
	Edge(int idSource, int idTarget) {
		this->idSource = idSource;
		this->idTarget = idTarget;
	}

	virtual ~Edge(){}
	
	/**
	 * Returns the source node of the edge
	 * @return the ID of the source node
	 */
	inline int getIdSource() {
		return idSource;
	}
	/**
	 * Returns the target node of the edge
	 * @return the ID of the target node
	 */
	inline int getIdTarget() {
		return idTarget;
	}
	
	/**
	 * Compares two edges
	 *
	 * @param e other edge to compare
	 * @return true if the edges are equal, false otherwise
	 */
	inline virtual bool equals (const Edge& e) {
		return ((e.idSource == this->idSource) && (e.idTarget == this->idTarget));
	}

	/**
	 * Returns a string with all the information of the edge, must be with a format that loadEdge can manage.
	 *
	 * @return a string with all the necessary information to store this edge
	 */
	inline virtual std::string toText() {
        return std::to_string(idSource) + "->" + std::to_string(idTarget) + ";";
	}

    inline virtual Edge* clone() {
        return new Edge(*this);
    }

	//SERIALIZATIoN
	template<class Archive>
	void serialize(Archive & ar, std::uint32_t const version);

protected:
	int idSource;
	int idTarget;

};

template<class Archive>
inline void Edge::serialize(Archive& ar, const std::uint32_t version) {
	if (version <= 1) {
		ar(CEREAL_NVP(idSource), CEREAL_NVP(idTarget));
	}
}
// Associate some type with a version number
CEREAL_CLASS_VERSION(Edge, (int)1);


#endif /* SRC_GRAPH_EDGE_H_ */
