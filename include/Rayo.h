#ifndef RAYO_H
#define RAYO_H
#include "Point.h"

using namespace std;

class Rayo{
    private:
        Point direccion;
        Point origen;

    public:
        Rayo(Point, Point);
        Point getDireccion();
        Point getOrigen();
};

#endif
