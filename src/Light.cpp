#include "../include/Light.h"

using namespace std;

Light::Light(Point position, Color color){
    this->color = color;
    this->position = position;
}

Point Light::getPosicion(){
    return position;
}

Color Light::getColor(){
    return color;
}
