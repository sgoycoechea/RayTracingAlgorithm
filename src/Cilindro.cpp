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

    // Ver si está en las tapas
    Point* centroTapa2 = (*centroBase) + ((*direccion) * altura);
    Point* vec1 = (*punto) - centroBase;
    Point* vec2 = (*punto) - centroTapa2;

    if (vec1->dotProduct(direccion) < 0.001 && vec1->dotProduct(direccion) > -0.001){
        return direccion;
    }

    if (vec2->dotProduct(direccion) < 0.001 && vec2->dotProduct(direccion) > -0.001){
        return (*direccion) * -1;
    }


    Point* centroPunto =  (*centroBase) - punto;
    // Restar altura del punto con respecto al cilindro
    Point* ret = (*centroPunto) - ((*direccion) * (centroPunto->dotProduct(direccion)));

    ret->normalizar();
    return ret;
}

double Cilindro::intersectarCuerpo(Rayo* rayo){
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

}

double Cilindro::intersectarTapa(Rayo* rayo, Point* centroTapa){
    double directions_dot_prod = direccion->dotProduct(rayo->getDireccion());

    if (directions_dot_prod == 0) {
        return FLT_MAX; // No hay interseccion, el plano del circulo y el rayo son paralelos
    }

    double ret = direccion->dotProduct((*centroTapa) - rayo->getOrigen()) / directions_dot_prod;

    if (ret < 0.01) { // Plano esta atras del rayo
        return FLT_MAX;
    }

    Point* interseccion = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * ret);

    Point* centroInterseccion = (*interseccion) - centroTapa;
    double distAlCentro = sqrt(centroInterseccion->dotProduct(centroInterseccion));


    if (distAlCentro < rad)
        return ret;

    // Intersecta con el plano pero no con el circulo
    return FLT_MAX;
}

double Cilindro::intersectar(Rayo* rayo) {

    Point* centroTapa2 = (*centroBase) + ((*direccion) * altura);

    double cuerpo = intersectarCuerpo(rayo);
    double tapa1 = intersectarTapa(rayo, centroBase);
    double tapa2 = intersectarTapa(rayo, centroTapa2) ;
    double tapaMasCerca = tapa1 < tapa2 ? tapa1 : tapa2;

    return cuerpo < tapaMasCerca ? cuerpo : tapaMasCerca;
};
