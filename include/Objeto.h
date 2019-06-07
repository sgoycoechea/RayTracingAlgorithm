#ifndef OBJETO_H
#define OBJETO_H

#include "Point.h"
#include "Color.h"
#include "Rayo.h"

using namespace std;

class Objeto {
    private:
        float opacidadR;
        float opacidadG;
        float opacidadB;
        Color* color;

    public:
      Objeto(float, float, float, Color*);
      virtual double intersectar(Rayo*) = 0;
      virtual Point* getNormal(Point* punto) = 0;
      virtual Color* getColor();
      virtual float getOpacidadR();
      virtual float getOpacidadG();
      virtual float getOpacidadB();
};

#endif
