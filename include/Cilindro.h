#ifndef CILINDRO_H
#define CILINDRO_H
#include "Point.h"
#include "Color.h"
#include "Rayo.h"
#include "Objeto.h"
#include <math.h>
#include <cfloat>

#define M_PI 3.1415926

using namespace std;

class Cilindro: public Objeto{
    private:
        Point* centroBase;
        Point* direccion;
        double rad;
        double altura;
        double intersectarCuerpo(Rayo*);
        double intersectarTapa(Rayo*, Point*);
    public:
        Cilindro(Point*, Point*, double, double, Color*, float, float, float);
        double intersectar(Rayo*);
        Point* getNormal(Point* punto);
};

#endif
