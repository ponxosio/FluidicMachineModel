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
    edgestr += "[label=\" direction=" + dirToStr() + ", cutted=" + std::to_string(cutted) + "\"];";
    return edgestr;
}

std::string TubeEdge::dirToStr() {
    std::string str = "";
    switch (direction) {
    case unknow:
        str = UNKNOW_STR;
        break;
    case regular:
        str = REGULAR_STR;
        break;
    case inverted:
        str = INVERTED_STR;
        break;
    default:
        break;
    }
    return str;
}
