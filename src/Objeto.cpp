#include "../include/Esfera.h"
#include <iostream>
#include <fstream>

using namespace std;

Objeto::Objeto(Color color, float coefTransmision, float coefEspecular, float coefDifuso, float coefAmbiente, float indiceRefraccion){
    this->color = color;
    this->coefDifuso = coefDifuso;
    this->coefEspecular = coefEspecular;
    this->coefTransmision = coefTransmision;
    this->indiceRefraccion = indiceRefraccion;
    this->coefAmbiente = coefAmbiente;
}

Color Objeto::getColor(){
    return Color(color.getR(), color.getG(), color.getB());
}

float Objeto::getCoefDifuso(){
    return coefDifuso;
}

float Objeto::getCoefAmbiente(){
    return coefAmbiente;
}

float Objeto::getCoefEspecular(){
    return coefEspecular;
}

float Objeto::getCoefTransmision(){
    return coefTransmision;
}

float Objeto::getIndiceRefraccion(){
    return indiceRefraccion;
}
