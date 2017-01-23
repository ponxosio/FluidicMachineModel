#ifndef TUBEEDGE_H
#define TUBEEDGE_H

#include "graph/Edge.h"

class TubeEdge : public Edge
{
public:
    typedef enum TubeDirection_ {
        unknow, //tube direction has not been set.
        regular, // tube direction = idSource->idTarget
        inverted // tube direction = idSource<-idTarget
    } TubeDirection;


    TubeEdge();
    TubeEdge(const TubeEdge & edge);
    TubeEdge(int idSource, int idTarget);
    virtual ~TubeEdge();

    virtual std::string toText();

    inline TubeDirection getDirection() {
        return direction;
    }

    inline void setDirection(TubeDirection dir) {
        this->direction = dir;
    }

    inline void cutEdge() {
        cutted = true;
    }

    inline bool isCutted() {
        return cutted;
    }

protected:
    bool cutted;
    TubeDirection direction;
};

#endif // TUBEEDGE_H
