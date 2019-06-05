#include "../include/Color.h"

using namespace std;

Color::Color(float r, float g, float b){
    this->r = r;
    this->g = g;
    this->b = b;
}

float Color::getR() {
  return r;
}

float Color::getG() {
  return g;
}

float Color::getB() {
  return b;
}

void Color::truncar() {
  float maximo = r;
  if (g > maximo)
    maximo = g;
  if (b > maximo)
    maximo = b;

  if (maximo > 255){
    float factor = 255 / maximo;
    r *= factor;
    g *= factor;
    b *= factor;
  }
}

Color* Color::escalar(float factor) {
  float newR = r * factor;
  if (newR > 255)
    newR = 255;
  float newG = g * factor;
  if (newG > 255)
    newG = 255;
  float newB = b * factor;
  if (newB > 255)
    newB = 255;

    return new Color(newR, newG, newB);
}

Color* Color::operator+(Color* color2){
    return new Color(r + color2->getR(), g + color2->getG(), b + color2->getB());
}

Color* Color::mezclar(Color* color2) {
  return new Color(r * color2->getR(), g * color2->getG(), b * color2->getB());
}
