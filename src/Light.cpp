#include "../include/Light.h"

using namespace std;

Luz::Luz(Point posicion, Color color){
    this->color = color;
    this->posicion = posicion;
}

Point Luz::getPosicion(){
    return posicion;
}

Color Luz::getColor(){
    return color;
}
