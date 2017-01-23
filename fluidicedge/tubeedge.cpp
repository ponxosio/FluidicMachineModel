#include "tubeedge.h"

TubeEdge::TubeEdge() :
    Edge()
{
    this->direction = unknow;
    cutted = false;
}

TubeEdge::TubeEdge(const TubeEdge & edge) :
    Edge(edge)
{
    this->direction = edge.direction;
    this->cutted = edge.cutted;
}

TubeEdge::TubeEdge(int idSource, int idTarget) :
    Edge(idSource, idTarget)
{
    this->direction = unknow;
    cutted = false;
}

TubeEdge::~TubeEdge() {

}

std::string TubeEdge::toText() {
    std::string edgestr = Edge::toText();
    edgestr = edgestr.substr(0, edgestr.size()-1);
    edgestr += "[label=\"" + std::to_string(flow) + "\"];";
    return edgestr;
}
