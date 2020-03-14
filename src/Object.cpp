#include "../include/Sphere.h"
#include <iostream>
#include <fstream>

using namespace std;

Object::Object(Color color, float transmissionCoefficient, float specularCoefficient, float diffusionCoefficient, float ambientCoefficient, float refractiveIndex){
    this->color = color;
    this->diffusionCoefficient = diffusionCoefficient;
    this->specularCoefficient = specularCoefficient;
    this->transmissionCoefficient = transmissionCoefficient;
    this->refractiveIndex = refractiveIndex;
    this->ambientCoefficient = ambientCoefficient;
}

Color Object::getColor(){
    return Color(color.getR(), color.getG(), color.getB());
}

float Object::getDiffusionCoefficient(){
    return diffusionCoefficient;
}

float Object::getAmbientCoefficient(){
    return ambientCoefficient;
}

float Object::getSpecularCoefficient(){
    return specularCoefficient;
}

float Object::getTransmissionCoefficient(){
    return transmissionCoefficient;
}

float Object::getRefractiveIndex(){
    return refractiveIndex;
}
