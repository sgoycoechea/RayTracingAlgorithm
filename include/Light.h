#ifndef LIGHT_H
#define LIGHT_H
#include "Point.h"
#include "Color.h"

using namespace std;

class Light{
    private:
        Point position;
        Color color;

    public:
        Light(Point, Color);
        Color getColor();
        Point getPosicion();
};

#endif
