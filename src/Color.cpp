#include "../include/Color.h"

using namespace std;

Color::Color(float r, float g, float b){
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(){
    this->r = 0;
    this->g = 0;
    this->b = 0;
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

void Color::truncate() {
  float maximum = r;
  if (g > maximum)
    maximum = g;
  if (b > maximum)
    maximum = b;

  if (maximum > 255){
    float factor = 255 / maximum;
    r *= factor;
    g *= factor;
    b *= factor;
  }
}

Color Color::scale(float factor) {
  float newR = r * factor;
  if (newR > 255)
    newR = 255;
  float newG = g * factor;
  if (newG > 255)
    newG = 255;
  float newB = b * factor;
  if (newB > 255)
    newB = 255;

    return Color(newR, newG, newB);
}

Color Color::operator+(Color color2){
    return Color(r + color2.getR(), g + color2.getG(), b + color2.getB());
}
