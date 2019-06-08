#include "FreeImage.h"
#include "include/Esfera.h"
#include "include/Cilindro.h"
#include "include/Plano.h"
#include "include/Objeto.h"
#include "include/Point.h"
#include "include/Rayo.h"
#include "include/Color.h"
#include "include/Luz.h"

#include <chrono>
#include <ctime>
#include <string>
#include <list>

#include <iostream>
#include <fstream>

using namespace std;

int writeFile(string txt)
{
  std::ofstream outfile;
  outfile.open("test.txt", std::ios_base::app);
  outfile << txt;
  return 0;
}


string getDate(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    string str(buffer);

    str[10] = '_';
    str[13] = '-';

    return str.substr(0,16);
}

Point* reflejar(Point* rayoLuz, Point* normal){
    float factor = 2 * normal->dotProduct(rayoLuz);
    Point* reflejado = (*normal) * factor;
    reflejado = (*reflejado) - rayoLuz;
    return reflejado;
}

Color* sombra_RR(Objeto* masCercano, Rayo* rayo, float distancia, Point* normal, list<Objeto*> objetos, list<Luz*> luces, int profundidad){
    Point* interseccion = (*rayo->getDireccion()) * distancia;
    Color* colorAmbiente = masCercano->getColor()->escalar(0.1);
    Color* colorDifuso = new Color(0,0,0);
    Color* colorEspecular = new Color(0,0,0);

    const float factorEspecular = 25;

    for (Luz* luz : luces) {
        Point* direccionLuz = (*interseccion) - luz->getPosicion();
        direccionLuz->normalizar();

        float prodInterno = normal->dotProduct(direccionLuz);

        if (prodInterno > 0){

            Point* vectorLuzObjeto = (*luz->getPosicion()) - interseccion;
            Rayo* rayoLuzObjeto = new Rayo(luz->getPosicion(), direccionLuz);
            float distanciaLuz = vectorLuzObjeto->magnitude();

            // Si el factor es 1 llega toda la luz, si es 0 no llega nada (hay otro objeto opaco en el medio)
            float factorR = 1;
            float factorG = 1;
            float factorB = 1;

            // Chequear si hay objetos en el medio
            for (Objeto* objeto : objetos) {
                if (objeto != masCercano){
                    float dist = objeto->intersectar(rayoLuzObjeto);
                    if (dist < distanciaLuz){
                        factorR *= 1 - objeto->getOpacidadR();
                        factorG *= 1 - objeto->getOpacidadG();
                        factorB *= 1 - objeto->getOpacidadB();
                    }
                }
            }

            // VER SI ESTA BIEN MULTIPLICAR POR factorR,G,B Y VER SI NO HAY QUE MULTIPLICAR POR masCercano->getOpacidadR()

            // Luz difusa
            Color* colorDifusoEstaLuz = new Color(luz->getColor()->getR() * masCercano->getColor()->getR() * factorR * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor()->getG() * masCercano->getColor()->getG() * factorG * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor()->getB() * masCercano->getColor()->getB() * factorB * prodInterno / (pow(distanciaLuz,2)));

            // Luz especular
            Point* luzReflejada = reflejar((*direccionLuz) * -1, normal);
            float prodInternoReflejado = pow(luzReflejada->dotProduct((*rayo->getDireccion()) * -1)  , factorEspecular);
            Color* colorEspecularEstaLuz = new Color(0,0,0);
            if (prodInternoReflejado > 0){
                colorEspecularEstaLuz = new Color(luz->getColor()->getR() * prodInternoReflejado * factorR,
                                                  luz->getColor()->getG() * prodInternoReflejado * factorG,
                                                  luz->getColor()->getB() * prodInternoReflejado* factorB);
            }

            colorDifuso = (*colorDifuso) + colorDifusoEstaLuz;
            colorEspecular = (*colorEspecular) + colorEspecularEstaLuz;
        }
    }

    Color* res = (*colorAmbiente) + colorDifuso;
    res = (*res) + colorEspecular;
    res->truncar();
    return res;
}

bool estaAntesEnLista(Objeto* objeto, Objeto* masCercano, list<Objeto*> objetos){

    for (Objeto* obj : objetos) {
        if (obj == objeto){
            return true;
        }
        if (obj == masCercano){
            return false;
        }
    }


    return false;
}

Color* traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, int profundidad){

    Color* color = new Color(0,0,0);
    rayo->getDireccion()->normalizar();
    Objeto* masCercano = nullptr;
    float distancia = FLT_MAX;

    for (Objeto* objeto : objetos) {
        float distObj = (*objeto).intersectar(rayo);
        if (distObj < distancia){
            if (distObj + 0.01 < distancia){
                masCercano = objeto;
                distancia = distObj;
            }
            else{

               if (estaAntesEnLista(objeto, masCercano, objetos)){

                   masCercano = objeto;
                   distancia = distObj;
               }
            }
        }
    }

    if (masCercano != nullptr){
        Point* interseccion = (*rayo->getDireccion()) * distancia;
        Point* normal = masCercano->getNormal(interseccion);
        normal->normalizar();
        return sombra_RR(masCercano, rayo, distancia, normal, objetos, luces, profundidad);
    }
    return color;
}

Point* productoVectorial(Point* vector1, Point* vector2){
    double x = vector1->getY() * vector2->getZ() - vector1->getZ() * vector2->getY();
    double y = - (vector1->getX() * vector2->getZ() - vector1->getZ() * vector2->getX());
    double z = vector1->getX() * vector2->getY() - vector1->getY() * vector2->getX();

    return new Point(x,y,z);
}


list<Objeto*> inicializarObjetos(){
    list<Objeto*> objetos;

    Objeto* cilindro = new Cilindro(new Point(1,-3,7), new Point(0,1,0), 0.7, 2, new Color(0,0,50), 1,1,1);
    Objeto* esfera = new Esfera(new Point(-1,-2.5,7), 0.5, new Color(0,20,0), 1,1,1);
    Objeto* paredFondo = new Plano(new Point(0,0,10), new Point(0,0,-1), new Color(250,250,250), 1,1,1);
    Objeto* piso = new Plano(new Point(0,-3,0), new Point(0,1,0), new Color(250,250,250), 1,1,1);
    Objeto* techo = new Plano(new Point(0,3,0), new Point(0,-1,0), new Color(150,150,150), 1,1,1);
    Objeto* paredIzq = new Plano(new Point(-3,0,0), new Point(1,0,0), new Color(200,0,0), 1,1,1);
    Objeto* paredDer = new Plano(new Point(3,0,0), new Point(-1,0,0), new Color(0,200,0), 1,1,1);

    objetos.push_back(cilindro);
    objetos.push_back(esfera);
    objetos.push_back(paredFondo);
    objetos.push_back(piso);
    objetos.push_back(techo);
    objetos.push_back(paredIzq);
    objetos.push_back(paredDer);

    return objetos;
}

list<Luz*> inicializarLuces(){
    list<Luz*> luces;

    Luz* luz1 = new Luz(new Point(2,2,1), new Color(200,200,200));
    Luz* luz2 = new Luz(new Point(0,1,2), new Color(200,200,200));

    luces.push_back(luz1);
    //luces.push_back(luz2);

    return luces;
}

int main() {
    // Tamaño de la imagen en pixeles
    float Height = 500;
    float Width = 500;

    // Settings camera: posicion, direccion y up
    Point* camara = new Point(0,0,0);
    Point* direccionCamara = new Point(0,0,1);
    Point* camaraUp = new Point(0,1,0);

    FIBITMAP *bitmap = FreeImage_Allocate(Width, Height, 32);
    string date = getDate();
    string path = "fotos/" + date + ".bmp";

    list<Luz*> luces = inicializarLuces();
    list<Objeto*> objetos = inicializarObjetos();

    Point* direccionLateral = (*productoVectorial(direccionCamara, camaraUp)) * -1;
    direccionCamara->normalizar();
    camaraUp->normalizar();
    direccionLateral->normalizar();

    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++)  {

        Point* direccionRayo = direccionCamara;
        direccionRayo = (*direccionRayo) + ((*direccionLateral) * (i / Height - 0.5)); // Mover horizontalmente
        direccionRayo = (*direccionRayo) + ((*camaraUp) * (j / Width - 0.5)); // Mover verticalmente
        direccionRayo->normalizar();
        Rayo* rayo = new Rayo(camara, direccionRayo);

        Color* color = traza_RR(rayo, objetos, luces, 1);

        RGBQUAD colorQuad;
        colorQuad.rgbRed = (int)color->getR();
        colorQuad.rgbGreen = (int)color->getG();
        colorQuad.rgbBlue = (int)color->getB();
        FreeImage_SetPixelColor(bitmap, i, j, &colorQuad);
        }
    }

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    return 0;
}
