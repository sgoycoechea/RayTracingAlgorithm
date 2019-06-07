#include "../include/Esfera.h"
#include <iostream>
#include <fstream>

using namespace std;

Objeto::Objeto(float opacidadR, float opacidadG, float opacidadB, Color* color){
    this->color = color;
    this->opacidadR = opacidadR;
    this->opacidadG = opacidadG;
    this->opacidadB = opacidadB;
    this->color = color;
}

Color* Objeto::getColor(){
    return new Color(color->getR(), color->getG(), color->getB());
}

float Objeto::getOpacidadR(){
    return opacidadR;
}

float Objeto::getOpacidadG(){
    return opacidadG;
}

float Objeto::getOpacidadB(){
    return opacidadB;
}
