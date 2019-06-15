#include "../include/Rayo.h"

using namespace std;

Rayo::Rayo(Point origen, Point direccion){
    this->origen = origen;
    this->direccion = direccion;
}

Point Rayo::getOrigen(){
    return origen;
}

Point Rayo::getDireccion(){
    return direccion;
}


