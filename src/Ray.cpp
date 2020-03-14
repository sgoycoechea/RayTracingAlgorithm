#include "../include/Ray.h"

using namespace std;

Ray::Ray(Point origen, Point direction){
    this->origen = origen;
    this->direction = direction;
}

Point Ray::getOrigin(){
    return origen;
}

Point Ray::getDirection(){
    return direction;
}


