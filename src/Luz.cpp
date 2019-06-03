#include "../include/Luz.h"

using namespace std;

Luz::Luz(Point* posicion, Color* color, float intensidad){
    this->color = color;
    this->posicion = posicion;
    this->intensidad = intensidad;
}

Point* Luz::getPosicion(){
    return posicion;
}

Color* Luz::getColor(){
    return color;
}


