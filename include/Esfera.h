#ifndef ESFERA_H
#define ESFERA_H
#include "Point.h"
#include "Rayo.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Esfera{
    private:
        Point* centro;
        double rad;

    public:
        Esfera(Point*, double);
        Point* getCentro();
        double getRad();
        double intersectar(Rayo*);
};

#endif
