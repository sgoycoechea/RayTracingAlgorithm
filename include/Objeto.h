#ifndef OBJETO_H
#define OBJETO_H

#include "Point.h"
#include "Color.h"
#include "Rayo.h"

using namespace std;

class Objeto {
    public:
      virtual double intersectar(Rayo*) = 0;
      virtual Point* getNormal(Point* punto) = 0;
      virtual Color* getColor() = 0;
      virtual float getOpacidadR() = 0;
      virtual float getOpacidadG() = 0;
      virtual float getOpacidadB() = 0;
};

#endif
