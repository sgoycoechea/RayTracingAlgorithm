#ifndef LUZ_H
#define LUZ_H
#include "Point.h"
#include "Color.h"

using namespace std;

class Luz{
    private:
        Point posicion;
        Color color;

    public:
        Luz(Point, Color);
        Color getColor();
        Point getPosicion();
};

#endif
