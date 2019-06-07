#include "../include/Cilindro.h"
#include <iostream>
#include <fstream>

using namespace std;

Cilindro::Cilindro(Point* centroBase, Point* direccion, double rad, double altura, Color* color, float opacidadR, float opacidadG, float opacidadB):Objeto(opacidadR, opacidadG, opacidadB, color){
    this->centroBase = centroBase;
    direccion->normalizar();
    this->direccion = direccion;
    this->altura = altura;
    this->rad = rad;
}

Point* Cilindro::getNormal(Point* punto){
    Point* centroPunto =  (*centroBase) - punto;
    // Restar altura del punto con respecto al cilindro
    Point* ret = (*centroPunto) - ((*direccion) * (centroPunto->dotProduct(direccion)));

    ret->normalizar();
    return ret;
}

double Cilindro::intersectar(Rayo* rayo) {
    Point* oriMenosCentro = (*rayo->getOrigen()) - centroBase;
    double direccionesDot = rayo->getDireccion()->dotProduct(direccion);

    //Bhaskaras
    double a = 1 - direccionesDot * direccionesDot;
    double b = 2 * (oriMenosCentro->dotProduct(rayo->getDireccion()) - direccionesDot * oriMenosCentro->dotProduct(direccion));
    double c = oriMenosCentro->dotProduct(oriMenosCentro) - oriMenosCentro->dotProduct(direccion) * oriMenosCentro->dotProduct(direccion) - rad * rad;
    double delta = b*b - 4 * a * c;

    if (delta < 0){
        return FLT_MAX; // No hay interseccion
    }

    double raiz1 = (-b - sqrt(delta)) / (2 * a);
    double raiz2 = (-b + sqrt(delta)) / (2 * a);

    if (raiz2 < 0.01) {
        return FLT_MAX; // El cilindro esta todo atras de la camara
    }

    // Hay interseccion con el cilindro infinito, falta chequear la altura del cilindro

    double proyeccionCentro = centroBase->dotProduct(direccion);

    Point* interseccion1 = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * raiz1);
    double proyeccionInter1 = interseccion1->dotProduct(direccion);
    if (raiz1 >= 0.01 && proyeccionInter1 > proyeccionCentro && proyeccionInter1 < proyeccionCentro + altura) {
        return raiz1;
    }

    Point* interseccion2 = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * raiz2);
    double proyeccionInter2 = interseccion2->dotProduct(direccion);
    if (raiz2 >= 0.01 && proyeccionInter2 > proyeccionCentro && proyeccionInter2 < proyeccionCentro + altura) {
        return raiz2;
    }

    return FLT_MAX; // No hay interseccion (habia interseccion con el cilindro infinito, pero no con el acotado por la altura)
};
