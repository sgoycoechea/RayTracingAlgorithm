#ifndef LUZ_H
#define LUZ_H
#include "Point.h"
#include "Color.h"

using namespace std;

class Luz{
    private:
        Point* posicion;
        Color* color;
        float intensidad;

    public:
        Luz(Point*, Color*, float);
        Color* getColor();
        Point* getPosicion();
        float getIntensidad();
};

#endif
