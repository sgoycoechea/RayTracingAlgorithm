#ifndef ESFERA_H
#define ESFERA_H
#include "Point.h"
#include "Color.h"
#include "Rayo.h"
#include "Objeto.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Esfera: public Objeto{
    private:
        Point* centro;
        Color* color;
        double rad;
        float opacidadR;
        float opacidadG;
        float opacidadB;
    public:
        Esfera(Point*, double, Color*, float, float, float);
        Point* getCentro();
        double getRad();
        Color* getColor();
        double intersectar(Rayo*);
        Point* getNormal(Point* punto);
        float getOpacidadR();
        float getOpacidadG();
        float getOpacidadB();
};

#endif
