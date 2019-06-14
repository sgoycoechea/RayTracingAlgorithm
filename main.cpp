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
/*
bool reflexionInternaTotal(Rayo* rayo, Point* normal, float n1, float n2){
    if (n1 > n2){
        float prodInterno = normal->dotProduct(rayo->getDireccion());
        float ang = acos(prodInterno);
        return ang > asin(n2/n1);
    } else return false;
}
*/
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
    if (profundidad < 8){

        if (masCercano->getCoefEspecular() > 0){

            Point* direcReflejada = reflejar(*rayo->getDireccion() * -1, normal);
            Point* interseccion = (*rayo->getOrigen()) + ((*rayo->getDireccion()) * (distancia - 0.0001));

            Rayo* rayo_r = new Rayo(interseccion, direcReflejada);
            Color* color_r = traza_RR (rayo_r, objetos, luces, objetosAtravezados, profundidad + 1);

            colorReflexion = color_r->escalar(masCercano->getCoefEspecular()) ;
        }

        if( masCercano->getCoefTransmision() > 0 ){

            normal->normalizar();
            rayo->getDireccion()->normalizar();

            Point* interseccion = *(rayo->getOrigen()) + (*(rayo->getDireccion()) * (distancia + 0.0001));
            float n1, n2;
/*
            Busco en el stack si ya esta el objeto con el que intersecté

                Si no está
                     n1 = 1 si el stack esta vacio, sino el que esta en la cima del stack
                     n2 = el indice del objeto intersectado
                     inserto

                Si está
                     n1 = al que está en la cima
                     saco del stack el objeto intersectado
                     n2 = 1 si el stack esta vacio, sino el que esta en la cima del stack
                     N = -N
*/


            vector<Objeto*>::iterator itr = find(objetosAtravezados.begin(), objetosAtravezados.end(), masCercano);

            if (itr != objetosAtravezados.end()){
            //if (find(objetosAtravezados.begin(), objetosAtravezados.end(), masCercano) != objetosAtravezados.end()) {

                //n1 = tope del stack
                n1 = objetosAtravezados.back()->getIndiceRefraccion();
                //saco del stack objeto intersectado
                objetosAtravezados.erase(itr);

                if (objetosAtravezados.empty())
                    n2 = 1;
                else
                    //n2 = tope del stack
                    n2 = objetosAtravezados.back()->getIndiceRefraccion();

                normal = *normal * -1;

            }else{
                if (objetosAtravezados.empty())
                    n1 = 1;
                else
                    //n1 = tope del stack
                    n1 = objetosAtravezados.back()->getIndiceRefraccion();

                n2 = masCercano->getIndiceRefraccion();

                objetosAtravezados.push_back(masCercano);
            }


            Point* N = normal;
            Point* I = rayo->getDireccion();

            //float ang1 = acos(N->dotProduct(I)); //es -I??
            //float ang2 = asin(sin(ang1) * n1 / n2);

            float ang1 = acos ( N->dotProduct(*I * -1));
            float ang2 = asin( n1*sin(ang1)/n2 );

            float angCritico = asin(n1/n2);


            // if (!reflexionInternaTotal(rayo, normal, n1, n2)){
            if(!(n1>n2 && ang1> angCritico)){

                //inmensia.com
                float n = n1 / n2;
                Point* direcRefractada = *(*I /n) + *N * ( (ang1/n) - sqrt(1 + (pow(ang1,2) - 1) / pow(n, 2) ));

             //ECUACIONES VIEJAS
/*
                float ang1 = acos(normal->dotProduct(rayo->getDireccion()) );
                float ang2 = asin( sin(ang1)* n1 / n2);
                Point* M = (*(*rayo->getDireccion() + (*normal * cos(ang1)))) / sin(ang1);
                Point* A = *M * sin(ang2);
                Point* B = *normal * -cos(ang2);

                Point* direcRefractada = *A + B;
*/



                Rayo* rayo_t = new Rayo(interseccion, direcRefractada);
                Color* color_t = traza_RR (rayo_t, objetos, luces, objetosAtravezados, profundidad + 1);
                colorRefraccion = color_t->escalar(masCercano->getCoefTransmision()) ;

            }else{
                Point* direcReflejado = reflejar(*rayo->getDireccion() * -1, normal);

                Rayo* rayo_Reflejado = new Rayo(interseccion, direcReflejado); //es interseccion lo que hay q poner?
                Color* color_t = traza_RR (rayo_Reflejado, objetos, luces, objetosAtravezados, profundidad + 1);
                colorRefraccion = color_t->escalar(masCercano->getCoefTransmision()) ;

            }

        }

    }

    Color* res = (*colorAmbiente) + colorDifuso;
    res = *res + colorEspecular;
    res = *res + colorRefraccion;
    res = *res + colorReflexion;
    res->truncar();
    return res;
}

Color* traza_RR(Rayo* rayo, list<Objeto*> objetos, list<Luz*> luces, vector<Objeto*> objetosAtravezados, int profundidad){

    Color* color = new Color(0,0,0);
    rayo->getDireccion()->normalizar();
    Objeto* masCercano = nullptr;
    float distancia = FLT_MAX;

    for (Objeto* objeto : objetos) {
        float distObj = (*objeto).intersectar(rayo);
        if (distObj < distancia + 0.001){
            distancia = distObj;
            masCercano = objeto;
            color = (*objeto).getColor();
        }
    }

    if (masCercano != nullptr){
        Point* interseccion = (*rayo->getDireccion()) * (distancia + 0.0001);
        Point* normal = masCercano->getNormal(interseccion);
        normal->normalizar();
        return sombra_RR(masCercano, rayo, distancia, normal, objetos, luces, objetosAtravezados, profundidad);
    }
    return color;
}

list<Objeto*> inicializarObjetos(){
    list<Objeto*> objetos;

    tinyxml2::XMLDocument doc;
    doc.LoadFile("escena.xml");


    // Esferas
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("esferas")->FirstChildElement("esfera"); child != 0; child = child->NextSiblingElement())
    {

        float x = stof(child->ToElement()->Attribute("centroX"), NULL);
        float y = stof(child->ToElement()->Attribute("centroY"), NULL);
        float z = stof(child->ToElement()->Attribute("centroZ"), NULL);

        float r = stof(child->ToElement()->Attribute("colorR"), NULL);
        float g = stof(child->ToElement()->Attribute("colorG"), NULL);
        float b = stof(child->ToElement()->Attribute("colorB"), NULL);

        float rad = stof(child->ToElement()->Attribute("radio"), NULL);

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"), NULL);
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"), NULL);
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"), NULL);
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"), NULL);

        Objeto* esfera = new Esfera(new Point(x,y,z), rad, new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(esfera);
    }

    // Cilindros

    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("cilindros")->FirstChildElement("cilindro"); child != 0; child = child->NextSiblingElement())

    {

        float centroX = stof(child->ToElement()->Attribute("centroBaseX"), NULL);
        float centroY = stof(child->ToElement()->Attribute("centroBaseY"), NULL);
        float centroZ = stof(child->ToElement()->Attribute("centroBaseZ"), NULL);

        float dirX = stof(child->ToElement()->Attribute("direccionX"), NULL);
        float dirY = stof(child->ToElement()->Attribute("direccionY"), NULL);
        float dirZ = stof(child->ToElement()->Attribute("direccionZ"), NULL);

        float rad = stof(child->ToElement()->Attribute("radio"), NULL);
        float alt = stof(child->ToElement()->Attribute("altura"), NULL);

        float r = stof(child->ToElement()->Attribute("colorR"), NULL);
        float g = stof(child->ToElement()->Attribute("colorG"), NULL);
        float b = stof(child->ToElement()->Attribute("colorB"), NULL);

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* cilindro = new Cilindro(new Point(centroX,centroY,centroZ), new Point(dirX,dirY,dirZ), rad, alt, new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(cilindro);
    }

    // Planos
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("planos")->FirstChildElement("plano"); child != 0; child = child->NextSiblingElement())
    {
        float puntoX = stof(child->ToElement()->Attribute("puntoX"), NULL);
        float puntoY = stof(child->ToElement()->Attribute("puntoY"), NULL);
        float puntoZ = stof(child->ToElement()->Attribute("puntoZ"), NULL);

        float normalX = stof(child->ToElement()->Attribute("normalX"), NULL);
        float normalY = stof(child->ToElement()->Attribute("normalY"), NULL);
        float normalZ = stof(child->ToElement()->Attribute("normalZ"), NULL);

        float r = stof(child->ToElement()->Attribute("colorR"), NULL);
        float g = stof(child->ToElement()->Attribute("colorG"), NULL);
        float b = stof(child->ToElement()->Attribute("colorB"), NULL);

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* plano = new Plano(new Point(puntoX,puntoY,puntoZ), new Point(normalX,normalY,normalZ), new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(plano);
    }

        // Triangulos
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("triangulos")->FirstChildElement("triangulo"); child != 0; child = child->NextSiblingElement())
    {
        float punto1X = stof(child->ToElement()->Attribute("punto1X"), NULL);
        float punto1Y = stof(child->ToElement()->Attribute("punto1Y"), NULL);
        float punto1Z = stof(child->ToElement()->Attribute("punto1Z"), NULL);

        float punto2X = stof(child->ToElement()->Attribute("punto2X"), NULL);
        float punto2Y = stof(child->ToElement()->Attribute("punto2Y"), NULL);
        float punto2Z = stof(child->ToElement()->Attribute("punto2Z"), NULL);

        float punto3X = stof(child->ToElement()->Attribute("punto3X"), NULL);
        float punto3Y = stof(child->ToElement()->Attribute("punto3Y"), NULL);
        float punto3Z = stof(child->ToElement()->Attribute("punto3Z"), NULL);

        float r = stof(child->ToElement()->Attribute("colorR"), NULL);
        float g = stof(child->ToElement()->Attribute("colorG"), NULL);
        float b = stof(child->ToElement()->Attribute("colorB"), NULL);

        float coefTransmision = stof(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stof(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stof(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stof(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* triangulo = new Triangulo(new Point(punto1X,punto1Y,punto1Z), new Point(punto2X,punto2Y,punto2Z), new Point(punto3X,punto3Y,punto3Z), new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(triangulo);
    }
  /*
    // Esferas
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("esferas")->FirstChildElement("esfera"); child != 0; child = child->NextSiblingElement())
    {

        float x = stoi(child->ToElement()->Attribute("centroX"));
        float y = stoi(child->ToElement()->Attribute("centroY"));
        float z = stoi(child->ToElement()->Attribute("centroZ"));

        float r = stoi(child->ToElement()->Attribute("colorR"));
        float g = stoi(child->ToElement()->Attribute("colorG"));
        float b = stoi(child->ToElement()->Attribute("colorB"));

        float rad = stoi(child->ToElement()->Attribute("radio"));

        float coefTransmision = stoi(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stoi(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stoi(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stoi(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* esfera = new Esfera(new Point(x,y,z), rad, new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(esfera);
    }

    // Cilindros

    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("cilindros")->FirstChildElement("cilindro"); child != 0; child = child->NextSiblingElement())

    {

        float centroX = stoi(child->ToElement()->Attribute("centroBaseX"));
        float centroY = stoi(child->ToElement()->Attribute("centroBaseY"));
        float centroZ = stoi(child->ToElement()->Attribute("centroBaseZ"));

        float dirX = stoi(child->ToElement()->Attribute("direccionX"));
        float dirY = stoi(child->ToElement()->Attribute("direccionY"));
        float dirZ = stoi(child->ToElement()->Attribute("direccionZ"));

        float rad = stoi(child->ToElement()->Attribute("radio"));
        float alt = stoi(child->ToElement()->Attribute("altura"));

        float r = stoi(child->ToElement()->Attribute("colorR"));
        float g = stoi(child->ToElement()->Attribute("colorG"));
        float b = stoi(child->ToElement()->Attribute("colorB"));

        float coefTransmision = stoi(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stoi(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stoi(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stoi(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* cilindro = new Cilindro(new Point(centroX,centroY,centroZ), new Point(dirX,dirY,dirZ), rad, alt, new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(cilindro);
    }

    // Planos
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("planos")->FirstChildElement("plano"); child != 0; child = child->NextSiblingElement())
    {
        float puntoX = stoi(child->ToElement()->Attribute("puntoX"));
        float puntoY = stoi(child->ToElement()->Attribute("puntoY"));
        float puntoZ = stoi(child->ToElement()->Attribute("puntoZ"));

        float normalX = stoi(child->ToElement()->Attribute("normalX"));
        float normalY = stoi(child->ToElement()->Attribute("normalY"));
        float normalZ = stoi(child->ToElement()->Attribute("normalZ"));

        float r = stoi(child->ToElement()->Attribute("colorR"));
        float g = stoi(child->ToElement()->Attribute("colorG"));
        float b = stoi(child->ToElement()->Attribute("colorB"));

        float coefTransmision = stoi(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stoi(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stoi(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stoi(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* plano = new Plano(new Point(puntoX,puntoY,puntoZ), new Point(normalX,normalY,normalZ), new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(plano);
    }

        // Triangulos
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objetos")->FirstChildElement("triangulos")->FirstChildElement("triangulo"); child != 0; child = child->NextSiblingElement())
    {
        float punto1X = stoi(child->ToElement()->Attribute("punto1X"));
        float punto1Y = stoi(child->ToElement()->Attribute("punto1Y"));
        float punto1Z = stoi(child->ToElement()->Attribute("punto1Z"));

        float punto2X = stoi(child->ToElement()->Attribute("punto2X"));
        float punto2Y = stoi(child->ToElement()->Attribute("punto2Y"));
        float punto2Z = stoi(child->ToElement()->Attribute("punto2Z"));

        float punto3X = stoi(child->ToElement()->Attribute("punto3X"));
        float punto3Y = stoi(child->ToElement()->Attribute("punto3Y"));
        float punto3Z = stoi(child->ToElement()->Attribute("punto3Z"));

        float r = stoi(child->ToElement()->Attribute("colorR"));
        float g = stoi(child->ToElement()->Attribute("colorG"));
        float b = stoi(child->ToElement()->Attribute("colorB"));

        float coefTransmision = stoi(child->ToElement()->Attribute("coefTransmision"));
        float coefEspecular = stoi(child->ToElement()->Attribute("coefEspecular"));
        float coefDifuso = stoi(child->ToElement()->Attribute("coefDifuso"));
        float indiceRefraccion = stoi(child->ToElement()->Attribute("indiceRefraccion"));

        Objeto* triangulo = new Triangulo(new Point(punto1X,punto1Y,punto1Z), new Point(punto2X,punto2Y,punto2Z), new Point(punto3X,punto3Y,punto3Z), new Color(r,g,b), coefTransmision, coefEspecular, coefDifuso, indiceRefraccion);
        objetos.push_back(triangulo);
    }
*/


     Objeto* esfera1 = new Esfera(new Point(-0.5, -1, 6), 0.8, new Color(100, 100, 0), 0 , 1, 0, 1.6);
     Objeto* esfera2 = new Esfera(new Point(-0.3, -0.3, 7), 0.7, new Color(100, 0, 0), 0.9, 0 , 0.1, 1.6);
     Objeto* esfera3 = new Esfera(new Point(1.2, -0.7, 7), 0.4, new Color(50, 0, 50), 0, 1 , 0, 1.3);

     Objeto* cilindro = new Cilindro(new Point(0.3,-1,7), new Point(0,1,0), 0.4, 0.9, new Color(0,50,0), 0, 0, 1, 1.6);
    objetos.push_back(cilindro);
    //objetos.push_back(esfera1);
    objetos.push_back(esfera2);
     objetos.push_back(esfera3);

    return objetos;
}

list<Luz*> inicializarLuces(){
    list<Luz*> luces;

    tinyxml2::XMLDocument doc;
    doc.LoadFile("escena.xml");

    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("luces")->FirstChildElement("luz"); child != 0; child = child->NextSiblingElement())
    {
        tinyxml2::XMLElement* posicionElement = child->FirstChildElement("posicion");
        tinyxml2::XMLElement* colorElement = child->FirstChildElement("color");

        float x = stoi(posicionElement->ToElement()->Attribute("x"));
        float y = stoi(posicionElement->ToElement()->Attribute("y"));
        float z = stoi(posicionElement->ToElement()->Attribute("z"));

        float r = stoi(colorElement->ToElement()->Attribute("r"));
        float g = stoi(colorElement->ToElement()->Attribute("g"));
        float b = stoi(colorElement->ToElement()->Attribute("b"));

        Luz* luz = new Luz(new Point(x,y,z), new Color(r,g,b));
        luces.push_back(luz);
    }

    return luces;
}


int main() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile("escena.xml");

    // Tamaño de la imagen en pixeles
    tinyxml2::XMLElement* resolucionElement = doc.FirstChildElement("file")->FirstChildElement("resolucion");
    float Height = stoi(resolucionElement->ToElement()->Attribute("x"));
    float Width = stoi(resolucionElement->ToElement()->Attribute("y"));

    // Inicializar camara
    tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("camara");
    tinyxml2::XMLElement* posElement = child->FirstChildElement("posicion");
    tinyxml2::XMLElement* dirElement = child->FirstChildElement("direccion");
    tinyxml2::XMLElement* upElement = child->FirstChildElement("up");
    Point* camaraPos = new Point(stoi(posElement->ToElement()->Attribute("x")), stoi(posElement->ToElement()->Attribute("y")), stoi(posElement->ToElement()->Attribute("z")));
    Point* camaraDir = new Point(stoi(dirElement->ToElement()->Attribute("x")), stoi(dirElement->ToElement()->Attribute("y")), stoi(dirElement->ToElement()->Attribute("z")));
    Point* camaraUp = new Point(stoi(upElement->ToElement()->Attribute("x")), stoi(upElement->ToElement()->Attribute("y")), stoi(upElement->ToElement()->Attribute("z")));

    FIBITMAP *bitmap = FreeImage_Allocate(Width, Height, 32);
    string date = getDate();
    string path = "fotos/" + date + ".bmp";

    list<Luz*> luces = inicializarLuces();
    list<Objeto*> objetos = inicializarObjetos();
    vector<Objeto*> objetosAtravezados;

    Point* direccionLateral = (*camaraDir->productoVectorial(camaraUp)) * -1;
    camaraDir->normalizar();
    camaraUp->normalizar();
    direccionLateral->normalizar();


    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++)  {

        Point* direccionRayo = camaraDir;
        direccionRayo = (*direccionRayo) + ((*direccionLateral) * (i / Height - 0.5)); // Mover horizontalmente
        direccionRayo = (*direccionRayo) + ((*camaraUp) * (j / Width - 0.5)); // Mover verticalmente
        direccionRayo->normalizar();
        Rayo* rayo = new Rayo(camaraPos, direccionRayo);

        Color* color = traza_RR(rayo, objetos, luces, objetosAtravezados, 0);

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
