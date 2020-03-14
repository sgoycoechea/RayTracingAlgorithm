#include "FreeImage.h"
#include "include/Sphere.h"
#include "include/Cylinder.h"
#include "include/Plain.h"
#include "include/Triangle.h"
#include "include/Object.h"
#include "include/Point.h"
#include "include/Ray.h"
#include "include/Color.h"
#include "include/Light.h"
#include "tinyxml2/tinyxml2.h"

#include <chrono>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;
using namespace tinyxml2;

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

Point reflect(Point ray, Point normal){
    float factor = 2 * normal.dotProduct(ray);
    Point reflejado = normal * factor;
    reflejado = reflejado - ray;
    return reflejado;
}

vector<Color> tracing_RR(Ray* ray, list<Object*> objects, list<Light*> lights, vector<Object*> crossedObjects, int recursionDepth);


Color shadow_RR(Object* closestObject, Ray* ray, float distance, Point normal, list<Object*> objects, list<Light*> lights, vector<Object*> crossedObjects, int recursionDepth){
    Point intersection = ray->getOrigin() + ray->getDirection() * distance;

    Color ambientColor = closestObject->getColor().scale(closestObject->getAmbientCoefficient());
    Color diffuseColor = Color(0,0,0);
    Color specularColor = Color(0,0,0);
    Color refractionColor = Color(0,0,0);
    Color reflectionColor = Color(0,0,0);
    const float factorN = 25;

    for (Light* light : lights) {
        Point directionLight = intersection - light->getPosicion();
        directionLight.normalize();

        float dotProduct = normal.dotProduct(directionLight);

        if (dotProduct > 0){

            Point vectorLightObject = light->getPosicion() - intersection;
            Ray* rayLightObject = new Ray(light->getPosicion(), directionLight);
            float lightDistance = vectorLightObject.magnitude();

            // If the factor is 1, all light gets to the object, if it's 0 no light gets to it (there is another object in the middle)
            float diffusionFactorR = closestObject->getDiffusionCoefficient();
            float diffusionFactorG = closestObject->getDiffusionCoefficient();
            float diffusionFactorB = closestObject->getDiffusionCoefficient();
            float specularFactorR = closestObject->getSpecularCoefficient();
            float specularFactorG = closestObject->getSpecularCoefficient();
            float specularFactorB = closestObject->getSpecularCoefficient();

            // Check of other objects are casting a shadow
            for (Object* object : objects) {
                if (object != closestObject){
                    float dist = object->intersect(rayLightObject);
                    if (dist + 0.001 < lightDistance){

                        int maxColor = object->getColor().getR() > object->getColor().getB() ? object->getColor().getR() : object->getColor().getG();
                        maxColor = maxColor > object->getColor().getB() ? maxColor : object->getColor().getB();
                        diffusionFactorR *= object->getTransmissionCoefficient() * (object->getColor().getR() / maxColor);
                        diffusionFactorG *= object->getTransmissionCoefficient() * (object->getColor().getG() / maxColor);
                        diffusionFactorB *= object->getTransmissionCoefficient() * (object->getColor().getB() / maxColor);
                        specularFactorR *= object->getTransmissionCoefficient() * (object->getColor().getR() / maxColor);
                        specularFactorG *= object->getTransmissionCoefficient() * (object->getColor().getG() / maxColor);
                        specularFactorB *= object->getTransmissionCoefficient() * (object->getColor().getB() / maxColor);

                        if (object->getTransmissionCoefficient() > 0){
                            diffusionFactorR *= 0.5;
                            diffusionFactorG *= 0.5;
                            diffusionFactorB *= 0.5;
                        }

                    }
                }
            }

            // Diffuse light
            Color lightDiffuseColor = Color(light->getColor().getR() * closestObject->getColor().getR() * diffusionFactorR * dotProduct / (pow(lightDistance,2)),
                                                light->getColor().getG() * closestObject->getColor().getG() * diffusionFactorG * dotProduct / (pow(lightDistance,2)),
                                                light->getColor().getB() * closestObject->getColor().getB() * diffusionFactorB * dotProduct / (pow(lightDistance,2)));

            // Specular light
            Point reflectedLight = reflect(directionLight * -1, normal);
            float reflectedDotProduct = pow(reflectedLight.dotProduct(ray->getDirection() * -1), factorN);
            Color lightSpecularColor = Color(0,0,0);
            if (reflectedDotProduct > 0){
                lightSpecularColor = Color(light->getColor().getR() * reflectedDotProduct * specularFactorR,
                                              light->getColor().getG() * reflectedDotProduct * specularFactorG,
                                              light->getColor().getB() * reflectedDotProduct * specularFactorB);
            }

            diffuseColor = diffuseColor + lightDiffuseColor;
            specularColor = specularColor + lightSpecularColor;
        }
    }

    if (recursionDepth < 5){

        // Reflection
        if (closestObject->getSpecularCoefficient() > 0){

            Point direcReflejada = reflect(ray->getDirection() * -1, normal);
            direcReflejada.normalize();

            Point intersection = ray->getOrigin() + ray->getDirection() * (distance - 0.0001);
            Ray* ray_r = new Ray(intersection, direcReflejada);

            Color color_r = (tracing_RR (ray_r, objects, lights, crossedObjects, recursionDepth + 1)).back();

            reflectionColor = color_r.scale(closestObject->getSpecularCoefficient()) ;
        }

        // Refraction
        if(closestObject->getTransmissionCoefficient() > 0){

            normal.normalize();
            ray->getDirection().normalize();

            Point intersection = ray->getOrigin() + ray->getDirection() * (distance + 0.0001);
            float n1, n2;
            vector<Object*>::iterator itr = find(crossedObjects.begin(), crossedObjects.end(), closestObject);

            if (itr != crossedObjects.end()){
                n1 = crossedObjects.back()->getRefractiveIndex();
                crossedObjects.erase(itr);

                if (crossedObjects.empty())
                    n2 = 1;
                else
                    n2 = crossedObjects.back()->getRefractiveIndex();

                normal = normal * -1;

            }

            else{
                if (crossedObjects.empty())
                    n1 = 1;
                else
                    n1 = crossedObjects.back()->getRefractiveIndex();

                n2 = closestObject->getRefractiveIndex();
                crossedObjects.push_back(closestObject);
            }


            Point N = normal;
            Point I = ray->getDirection();
            N.normalize();
            I.normalize();

            float ang1 = acos ( N.dotProduct(I * -1));
            float criticalAngle = asin(n1/n2);

            if(!(n1 > n2 && ang1 > criticalAngle)){

                float ang2 = asin( sin(ang1)* n1 / n2);
                Point M = (ray->getDirection() + normal * cos(ang1)) / sin(ang1);
                Point A = M * sin(ang2);
                Point B = normal * cos(ang2);

                Point refractedDirection = A + B;
                refractedDirection.normalize();

                Ray* ray_t = new Ray(intersection, refractedDirection);
                Color color_t = (tracing_RR (ray_t, objects, lights, crossedObjects, recursionDepth + 1)).back();

                refractionColor = color_t.scale(closestObject->getTransmissionCoefficient()) ;
                delete ray_t;

            }

            else{
                Point reflectedDirection = reflect(ray->getDirection() * -1, normal);
                reflectedDirection.normalize();

                Ray* reflectedRay = new Ray(intersection, reflectedDirection);
                Color color_t = (tracing_RR (reflectedRay, objects, lights, crossedObjects, recursionDepth + 1)).back();

                refractionColor = color_t.scale(closestObject->getTransmissionCoefficient()) ;
                delete reflectedRay;
            }
        }
    }

    Color res = ambientColor + diffuseColor + specularColor + reflectionColor + refractionColor;
    res.truncate();
    return res;
}

// Returns three colors, one for the common image, and one for each of the black and white images
vector<Color> tracing_RR(Ray* ray, list<Object*> objects, list<Light*> lights, vector<Object*> crossedObjects, int recursionDepth){

    Color color = Color(0,0,0);
    float coefT = 0;
    float coefR = 0;
    Color colorT = Color(250,250,250);
    Color colorR = Color(250,250,250);
    ray->getDirection().normalize();
    Object* closestObject = nullptr;
    float distance = FLT_MAX;

    for (Object* object : objects) {
        float objectDistance = (*object).intersect(ray);
        if (objectDistance < distance){
            distance = objectDistance;
            closestObject = object;
            color = (*object).getColor();
            coefT = (*object).getTransmissionCoefficient();
            coefR = (*object).getSpecularCoefficient();
        }
    }
    colorT = colorT.scale(coefT);
    colorR = colorR.scale(coefR);

    if (closestObject != nullptr){
        Point intersection = ray->getOrigin() + ray->getDirection() * distance;
        Point normal = closestObject->getNormal(intersection);
        normal.normalize();
        color = shadow_RR(closestObject, ray, distance, normal, objects, lights, crossedObjects, recursionDepth);
    }

    vector<Color> colores;
    colores.push_back(colorT);
    colores.push_back(colorR);
    colores.push_back(color);

    return colores;
}


list<Object*> initializeObjects(){
    list<Object*> objects;

    tinyxml2::XMLDocument doc;
    doc.LoadFile("scene.xml");

    // Spheres
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objects")->FirstChildElement("spheres")->FirstChildElement("sphere"); child != 0; child = child->NextSiblingElement())
    {

        float x = stof(child->ToElement()->Attribute("centerX"));
        float y = stof(child->ToElement()->Attribute("centerY"));
        float z = stof(child->ToElement()->Attribute("centerZ"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float radius = stof(child->ToElement()->Attribute("radius"));

        float transmissionCoefficient = stof(child->ToElement()->Attribute("transmissionCoefficient"));
        float specularCoefficient = stof(child->ToElement()->Attribute("specularCoefficient"));
        float diffusionCoefficient = stof(child->ToElement()->Attribute("diffusionCoefficient"));
        float ambientCoefficient = stof(child->ToElement()->Attribute("ambientCoefficient"));
        float refractiveIndex = stof(child->ToElement()->Attribute("refractiveIndex"));

        Object* sphere = new Sphere(Point(x,y,z), radius, Color(r,g,b), transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex);
        objects.push_back(sphere);
    }

    // Cylinders
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objects")->FirstChildElement("cylinders")->FirstChildElement("cylinder"); child != 0; child = child->NextSiblingElement())
    {
        float centerX = stof(child->ToElement()->Attribute("baseCenterX"));
        float centerY = stof(child->ToElement()->Attribute("baseCenterY"));
        float centerZ = stof(child->ToElement()->Attribute("baseCenterZ"));

        float directionX = stof(child->ToElement()->Attribute("directionX"));
        float directionY = stof(child->ToElement()->Attribute("directionY"));
        float directionZ = stof(child->ToElement()->Attribute("directionZ"));

        float radius = stof(child->ToElement()->Attribute("radius"));
        float alt = stof(child->ToElement()->Attribute("height"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float transmissionCoefficient = stof(child->ToElement()->Attribute("transmissionCoefficient"));
        float specularCoefficient = stof(child->ToElement()->Attribute("specularCoefficient"));
        float diffusionCoefficient = stof(child->ToElement()->Attribute("diffusionCoefficient"));
        float ambientCoefficient = stof(child->ToElement()->Attribute("ambientCoefficient"));
        float refractiveIndex = stof(child->ToElement()->Attribute("refractiveIndex"));

        Object* cylinder = new Cylinder(Point(centerX,centerY,centerZ), Point(directionX,directionY,directionZ), radius, alt, Color(r,g,b), transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex);
        objects.push_back(cylinder);
    }

    // Plains
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objects")->FirstChildElement("plains")->FirstChildElement("plain"); child != 0; child = child->NextSiblingElement())
    {
        float pointX = stof(child->ToElement()->Attribute("pointX"));
        float pointY = stof(child->ToElement()->Attribute("pointY"));
        float pointZ = stof(child->ToElement()->Attribute("pointZ"));

        float normalX = stof(child->ToElement()->Attribute("normalX"));
        float normalY = stof(child->ToElement()->Attribute("normalY"));
        float normalZ = stof(child->ToElement()->Attribute("normalZ"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float transmissionCoefficient = stof(child->ToElement()->Attribute("transmissionCoefficient"));
        float specularCoefficient = stof(child->ToElement()->Attribute("specularCoefficient"));
        float diffusionCoefficient = stof(child->ToElement()->Attribute("diffusionCoefficient"));
        float ambientCoefficient = stof(child->ToElement()->Attribute("ambientCoefficient"));
        float refractiveIndex = stof(child->ToElement()->Attribute("refractiveIndex"));

        Object* plain = new Plain(Point(pointX,pointY,pointZ), Point(normalX,normalY,normalZ), Color(r,g,b), transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex);
        objects.push_back(plain);
    }

    // Triangles
    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("objects")->FirstChildElement("triangles")->FirstChildElement("triangle"); child != 0; child = child->NextSiblingElement())
    {
        float pointA1 = stof(child->ToElement()->Attribute("pointA1"));
        float pointA2 = stof(child->ToElement()->Attribute("pointA2"));
        float pointA3 = stof(child->ToElement()->Attribute("pointA3"));

        float pointB1 = stof(child->ToElement()->Attribute("pointB1"));
        float pointB2 = stof(child->ToElement()->Attribute("pointB2"));
        float pointB3 = stof(child->ToElement()->Attribute("pointB3"));

        float pointC1 = stof(child->ToElement()->Attribute("pointC1"));
        float pointC2 = stof(child->ToElement()->Attribute("pointC2"));
        float pointC3 = stof(child->ToElement()->Attribute("pointC3"));

        float r = stof(child->ToElement()->Attribute("colorR"));
        float g = stof(child->ToElement()->Attribute("colorG"));
        float b = stof(child->ToElement()->Attribute("colorB"));

        float transmissionCoefficient = stof(child->ToElement()->Attribute("transmissionCoefficient"));
        float specularCoefficient = stof(child->ToElement()->Attribute("specularCoefficient"));
        float diffusionCoefficient = stof(child->ToElement()->Attribute("diffusionCoefficient"));
        float ambientCoefficient = stof(child->ToElement()->Attribute("ambientCoefficient"));
        float refractiveIndex = stof(child->ToElement()->Attribute("refractiveIndex"));

        Object* triangle = new Triangle(Point(pointA1,pointA2,pointA3), Point(pointB1,pointB2,pointB3), Point(pointC1,pointC2,pointC3), Color(r,g,b), transmissionCoefficient, specularCoefficient, diffusionCoefficient, ambientCoefficient, refractiveIndex);
        objects.push_back(triangle);
    }

    return objects;
}

list<Light*> initializeLights(){
    list<Light*> lights;

    tinyxml2::XMLDocument doc;
    doc.LoadFile("scene.xml");

    for(tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("lights")->FirstChildElement("light"); child != 0; child = child->NextSiblingElement())
    {
        tinyxml2::XMLElement* positionElement = child->FirstChildElement("position");
        tinyxml2::XMLElement* colorElement = child->FirstChildElement("color");

        float x = stoi(positionElement->ToElement()->Attribute("x"));
        float y = stoi(positionElement->ToElement()->Attribute("y"));
        float z = stoi(positionElement->ToElement()->Attribute("z"));

        float r = stoi(colorElement->ToElement()->Attribute("r"));
        float g = stoi(colorElement->ToElement()->Attribute("g"));
        float b = stoi(colorElement->ToElement()->Attribute("b"));

        Light* light = new Light(Point(x,y,z), Color(r,g,b));
        lights.push_back(light);
    }

    return lights;
}


int main() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile("scene.xml");

    // Size of the image in pixels
    tinyxml2::XMLElement* resolutionElement = doc.FirstChildElement("file")->FirstChildElement("resolution");
    float height = stoi(resolutionElement->ToElement()->Attribute("x"));
    float width = stoi(resolutionElement->ToElement()->Attribute("y"));

    // Initialize camera
    tinyxml2::XMLElement* child = doc.FirstChildElement("file")->FirstChildElement("camera");
    tinyxml2::XMLElement* posElement = child->FirstChildElement("position");
    tinyxml2::XMLElement* dirElement = child->FirstChildElement("direction");
    tinyxml2::XMLElement* upElement = child->FirstChildElement("up");

    Point cameraPosition = Point(stof(posElement->ToElement()->Attribute("x")), stof(posElement->ToElement()->Attribute("y")), stof(posElement->ToElement()->Attribute("z")));
    Point cameraDir = Point(stof(dirElement->ToElement()->Attribute("x")), stof(dirElement->ToElement()->Attribute("y")), stof(dirElement->ToElement()->Attribute("z")));
    Point cameraUp = Point(stof(upElement->ToElement()->Attribute("x")), stof(upElement->ToElement()->Attribute("y")), stof(upElement->ToElement()->Attribute("z")));

    Point lateralDirection = cameraDir.crossProduct(cameraUp) * -1;
    cameraDir.normalize();
    cameraUp.normalize();
    lateralDirection.normalize();

    // Create images
    FIBITMAP *bitmap = FreeImage_Allocate(width, height, 32);
    string date = getDate();
    string path = "images/" + date + ".bmp";

    FIBITMAP *bitmapT = FreeImage_Allocate(width, height, 32);
    string t = "transmission";
    string pathT = "images/" + t  + ".bmp";

    FIBITMAP *bitmapR = FreeImage_Allocate(width, height, 32);
    string r = "reflection";
    string pathR = "images/" + r  + ".bmp";

    // Initialize lights y objects
    list<Light*> lights = initializeLights();
    list<Object*> objects = initializeObjects();
    vector<Object*> crossedObjects;

    // For each pixel
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++)  {

            Point rayDirection = cameraDir;
            rayDirection = rayDirection + (lateralDirection * (i / height - 0.5)); // Move horizontally
            rayDirection = rayDirection + (cameraUp * (j / width - 0.5)); // Move vertically
            rayDirection.normalize();
            Ray* ray = new Ray(cameraPosition, rayDirection);

            vector<Color> colores = tracing_RR(ray, objects, lights, crossedObjects, 0);
            Color color = colores.back();

            RGBQUAD colorQuad;
            colorQuad.rgbRed = (int)color.getR();
            colorQuad.rgbGreen = (int)color.getG();
            colorQuad.rgbBlue = (int)color.getB();
            FreeImage_SetPixelColor(bitmap, i, j, &colorQuad);

            // Black and white images
            colores.pop_back();
            Color colorR = colores.back();

            RGBQUAD colorQuadR;
            colorQuadR.rgbRed = (int)colorR.getR();
            colorQuadR.rgbGreen = (int)colorR.getG();
            colorQuadR.rgbBlue = (int)colorR.getB();
            FreeImage_SetPixelColor(bitmapR, i, j, &colorQuadR);

            colores.pop_back();
            Color colorT = colores.back();

            RGBQUAD colorQuadT;
            colorQuadT.rgbRed = (int)colorT.getR();
            colorQuadT.rgbGreen = (int)colorT.getG();
            colorQuadT.rgbBlue = (int)colorT.getB();
            FreeImage_SetPixelColor(bitmapT, i, j, &colorQuadT);
        }
    }

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);
    FreeImage_Save(FIF_BMP, bitmapT, pathT.c_str(), 0);
    FreeImage_Save(FIF_BMP, bitmapR, pathR.c_str(), 0);

    return 0;
}
