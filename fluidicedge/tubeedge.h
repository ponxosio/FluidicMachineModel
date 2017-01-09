#ifndef TUBEEDGE_H
#define TUBEEDGE_H

#include "graph/Edge.h"

class TubeEdge : public Edge
{
public:
    TubeEdge();
    TubeEdge(const TubeEdge & edge);
    TubeEdge(int idSource, int idTarget);
    virtual ~TubeEdge();

    virtual std::string toText();

    inline int getFlow() {
        return flow;
    }

    inline void setFlow(int flow) {
        this->flow = flow;
    }
protected:
    int flow;
};

#endif // TUBEEDGE_H
