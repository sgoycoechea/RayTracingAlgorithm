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

void Color::escalar(float factor) {
  r = r * factor;
  if (r > 255)
    r = 255;
  g = g * factor;
  if (b > 255)
    b = 255;
  b = b * factor;
  if (g > 255)
    g = 255;
}
