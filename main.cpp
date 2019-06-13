#include "FreeImage.h"
#include "include/Esfera.h"
#include "include/Cilindro.h"
#include "include/Plano.h"
#include "include/Triangulo.h"
#include "include/Objeto.h"
#include "include/Point.h"
#include "include/Rayo.h"
#include "include/Color.h"
#include "include/Luz.h"
#include "tinyxml2/tinyxml2.h"


#include <chrono>
#include <ctime>
#include <string>
#include <list>
#include <vector>
#include <algorithm>


#include <iostream>
#include <fstream>

using namespace std;
using namespace tinyxml2;

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

bool reflexionInternaTotal(Rayo* rayo, Point* normal, float n1, float n2){
    if (n1 > n2){
        float prodInterno = normal->dotProduct(rayo->getDireccion());
        float ang = acos(prodInterno);
        return ang > asin(n2/n1);
    } else return false;
}

Color* traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados, int profundidad);


Color* sombra_RR(Objeto* masCercano, Rayo* rayo, float distancia, Point* normal, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados,int profundidad){
    Point* interseccion = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * distancia);
    Color* colorAmbiente = masCercano->getColor()->escalar(0.1);
    Color* colorDifuso = new Color(0,0,0);
    Color* colorEspecular = new Color(0,0,0);
    Color* colorRefraccion = new Color(0,0,0);
    Color* colorReflexion = new Color(0,0,0);
    const float factorN = 25;

    for (Luz* luz : luces) {
        Point* direccionLuz = (*interseccion) - luz->getPosicion();
        direccionLuz->normalizar();

        float prodInterno = normal->dotProduct(direccionLuz);

        if (prodInterno > 0){

            Point* vectorLuzObjeto = (*luz->getPosicion()) - interseccion;
            Rayo* rayoLuzObjeto = new Rayo(luz->getPosicion(), direccionLuz);
            float distanciaLuz = vectorLuzObjeto->magnitude();

            // Si el factor es 1 llega toda la luz, si es 0 no llega nada (hay otro objeto opaco en el medio)
            float factorDifuso = masCercano->getCoefDifuso();
            float factorEspecular = masCercano->getCoefEspecular();

            // Chequear si hay objetos en el medio
            for (Objeto* objeto : objetos) {
                if (objeto != masCercano){
                    float dist = objeto->intersectar(rayoLuzObjeto);
                    if (dist + 0.001 < distanciaLuz){
                        factorDifuso *= objeto->getCoefTransmision(); //no es 1 - ... ?? xq ahora cuando la transmision = 1 no se escala el factor
                        factorEspecular *= objeto->getCoefTransmision();
                    }
                }
            }

            // VER SI ESTA BIEN MULTIPLICAR POR factorR,G,B Y VER SI NO HAY QUE MULTIPLICAR POR masCercano->getOpacidadR()

            // Luz difusa
            Color* colorDifusoEstaLuz = new Color(luz->getColor()->getR() * masCercano->getColor()->getR() * factorDifuso * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor()->getG() * masCercano->getColor()->getG() * factorDifuso * prodInterno / (pow(distanciaLuz,2)),
                                            luz->getColor()->getB() * masCercano->getColor()->getB() * factorDifuso * prodInterno / (pow(distanciaLuz,2)));

            // Luz especular
            Point* luzReflejada = reflejar((*direccionLuz) * -1, normal);
            float prodInternoReflejado = pow(luzReflejada->dotProduct((*rayo->getDireccion()) * -1)  , factorN);
            Color* colorEspecularEstaLuz = new Color(0,0,0);
            if (prodInternoReflejado > 0){
                colorEspecularEstaLuz = new Color(luz->getColor()->getR() * prodInternoReflejado * factorEspecular,
                                                  luz->getColor()->getG() * prodInternoReflejado * factorEspecular,
                                                  luz->getColor()->getB() * prodInternoReflejado* factorEspecular);
            }

            colorDifuso = (*colorDifuso) + colorDifusoEstaLuz;
            colorEspecular = (*colorEspecular) + colorEspecularEstaLuz;
        }
    }
    if (profundidad < 5){

        if (masCercano->getCoefEspecular() > 0){

            Point* direcReflejada = reflejar(*rayo->getDireccion() * -1, normal);
            Point* interseccion = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * (distancia - 0.01));

            Rayo* rayo_r = new Rayo(interseccion, direcReflejada);
            Color* color_r = traza_RR (rayo_r, objetos, luces, objetosAtravezados, profundidad + 1);

            colorReflexion = color_r ->escalar(masCercano->getCoefEspecular()) ;



        }



        if( masCercano->getCoefTransmision() > 0 ){

            if ( find(objetosAtravezados.begin(), objetosAtravezados.end(), masCercano) != objetosAtravezados.end() )
                objetosAtravezados.pop_back();
            else
                objetosAtravezados.push_back(masCercano);

            float n1, n2;
            /*n1 = 1;
            n2 = 1.5;
            */

            if (!objetosAtravezados.empty()){
                n1 = 1;
                n2 = masCercano->getIndiceRefraccion();
            }else{
                n1 = masCercano->getIndiceRefraccion();
                n2 = 1;
                //n1 = objetosAtravezados.back()->getIndiceRefraccion();
            }

            if (!reflexionInternaTotal(rayo, normal, n1, n2)){
  /*            //ECUACIONES VIEJAS
                normal->normalizar();
                rayo->getDireccion()->normalizar();

                float ang1 = acos(normal->dotProduct(rayo->getDireccion()) );
                float ang2 = asin( sin(ang1)* n1 / n2);
                Point* M = (*(*rayo->getDireccion() + (*normal * cos(ang1)))) / sin(ang1);
                Point* A = *M * sin(ang2);
                Point* B = *normal * -cos(ang2);

                Point* direcRefractada = *A + B;

*/              //ECUACIONES NUEVAS
                rayo->getDireccion()->normalizar();
                normal->normalizar();
                float nr = n1/n2;
                float raiz = sqrt(1 - ( pow(nr,2) * (1 - pow((normal->dotProduct(rayo->getDireccion())),2)) ) );
                float factorAux = nr * (normal->dotProduct(rayo->getDireccion())) - raiz;

                Point* direcRefractada =  *(*normal * factorAux) - (*rayo->getDireccion() * nr);
                //---------------------------------------

                Point* interseccion = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * (distancia + 0.01));

                Rayo* rayo_t = new Rayo(interseccion, direcRefractada);
                Color* color_t = traza_RR (rayo_t, objetos, luces, objetosAtravezados, profundidad + 1);

                colorRefraccion = color_t ->escalar(masCercano->getCoefTransmision()) ;

            }else{
/*
                Point* direcReflejado = reflejar(rayo->getDireccion(), normal);
                Point* nuevaInterseccion = rayo->getOrigen() + (*rayo->getDireccion()) * (distancia + 0.1); //YA ESTA ARRIBA HAY Q PONERLO GLOBAL como interseccion

                Rayo* rayo_Reflejado = new Rayo(nuevaInterseccion, direcReflejado); //es interseccion lo que hay q poner?
                Color* color_t = traza_RR (rayo_Reflejado, objetos, luces, objetosAtravezados, profundidad + 1);
                colorRefraccion = color_t ->escalar(masCercano->getCoefTransmision()) ;
*/
            }

        }

    }

    Color* res = (*colorAmbiente) + colorDifuso;
    res = *res + colorEspecular;
    //res = *res + colorRefraccion;
    res = *res + colorReflexion;
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

Color* traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados, int profundidad){

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
        Point* interseccion = (*rayo->getDireccion()) * (distancia - 0.0001);
        Point* normal = masCercano->getNormal(interseccion);
        normal->normalizar();
        return sombra_RR(masCercano, rayo, distancia, normal, objetos, luces, objetosAtravezados, profundidad);
    }
    return color;
}


list<Objeto*> inicializarObjetos(){
    list<Objeto*> objetos;

    Objeto* cilindro = new Cilindro(new Point(1,-3,7), new Point(0,1,0), 0.9, 2, new Color(0,0,50), 0, 0, 1, 1);
    Objeto* esfera = new Esfera(new Point(-1,-2.5,7), 0.5, new Color(0,20,0), 0, 1, 0, 1);
    Objeto* paredFondo = new Plano(new Point(0,0,10), new Point(0,0,-1), new Color(250,250,250), 0, 0, 1, 1);
    Objeto* piso = new Plano(new Point(0,-3,0), new Point(0,1,0), new Color(250,250,250), 0, 0, 1, 1);
    Objeto* techo = new Plano(new Point(0,3,0), new Point(0,-1,0), new Color(150,150,150),0, 0, 1, 1);
    Objeto* paredIzq = new Plano(new Point(-3,0,0), new Point(1,0,0), new Color(200,0,0), 0, 0, 1, 1);
    Objeto* paredDer = new Plano(new Point(3,0,0), new Point(-1,0,0), new Color(0,200,0), 0, 0, 1, 1);
    Objeto* triangulo = new Triangulo(new Point(1,1,9), new Point(-1,1,8), new Point(0,-1,8), new Color(50,30,30),0, 0, 1, 1);

    //Objeto* esfera1 = new Esfera(new Point(-0.5, -1, 5), 0.8, new Color(20, 20, 20), 0, 0.8, 0.2, 1.5);
    //Objeto* esfera3 = new Esfera(new Point(1.3, -1, 5), 0.5, new Color(0, 0, 20), 0, 0.8, 0.2, 1.5);

    objetos.push_back(cilindro);
    objetos.push_back(esfera);
    //objetos.push_back(esfera1);
    //objetos.push_back(esfera3);
    objetos.push_back(paredFondo);
    objetos.push_back(piso);
    objetos.push_back(techo);
    objetos.push_back(paredIzq);
    objetos.push_back(paredDer);
    objetos.push_back(triangulo);


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

    XMLDocument doc;

    // Settings camera: posicion, direccion y up
    Point* camara = new Point(0,0,0);
    Point* direccionCamara = new Point(0,0,1);
    Point* camaraUp = new Point(0,1,0);

    FIBITMAP *bitmap = FreeImage_Allocate(Width, Height, 32);
    string date = getDate();
    string path = "fotos/" + date + ".bmp";

    list<Luz*> luces = inicializarLuces();
    list<Objeto*> objetos = inicializarObjetos();
    vector<Objeto*> objetosAtravezados;

    Point* direccionLateral = (*direccionCamara->productoVectorial(camaraUp)) * -1;
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

        Color* color = traza_RR(rayo, objetos, luces, objetosAtravezados, 1);

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
