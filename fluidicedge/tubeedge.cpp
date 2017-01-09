#include "tubeedge.h"

TubeEdge::TubeEdge() :
    Edge()
{
    this->flow = 0;
}

TubeEdge::TubeEdge(const TubeEdge & edge) :
    Edge(edge)
{
    this->flow = edge.flow;
}

TubeEdge::TubeEdge(int idSource, int idTarget) :
    Edge(idSource, idTarget)
{
    this->flow = 0;
}

TubeEdge::~TubeEdge() {

}

std::string TubeEdge::toText() {
    std::string edgestr = Edge::toText();
    edgestr = edgestr.substr(0, edgestr.size()-1);
    edgestr += "[label=\"" + std::to_string(flow) + "\"];";
    return edgestr;
}
