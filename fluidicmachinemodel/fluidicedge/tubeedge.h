#ifndef TUBEEDGE_H
#define TUBEEDGE_H

#define UNKNOW_STR "Unknow"
#define REGULAR_STR "Regular"
#define INVERTED_STR "Inverted"

#include <graph/Edge.h>

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

    inline void uncutEdge() {
        cutted = false;
    }
    inline bool isCutted() {
        return cutted;
    }

    inline virtual Edge* clone() {
        return new TubeEdge(*this);
    }

protected:
    bool cutted;
    TubeDirection direction;

    std::string dirToStr();
};

#endif // TUBEEDGE_H
