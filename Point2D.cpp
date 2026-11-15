#include "Point2D.hpp"
#include "PrimitiveRenderer.hpp"
#include <cmath>

Point2D::Point2D(float x, float y, sf::Color color)
    : x(x), y(y), color(color) {}

float Point2D::getX() const { return x; }
float Point2D::getY() const { return y; }
sf::Color Point2D::getColor() const { return color; }

void Point2D::setX(float newX) { x = newX; }
void Point2D::setY(float newY) { y = newY; }
void Point2D::setColor(sf::Color newColor) { color = newColor; }

void Point2D::draw(PrimitiveRenderer& renderer) {
    renderer.drawPoint(*this);
}

void Point2D::translate(float tx, float ty) {
    x += tx;
    y += ty;
}

void Point2D::rotate(float angleDegrees, float x0, float y0) {
    float radians = angleDegrees * 3.14159265359f / 180.0f;
    float cosAngle = std::cos(radians);
    float sinAngle = std::sin(radians);

    float dx = x - x0;
    float dy = y - y0;

    x = x0 + (dx * cosAngle - dy * sinAngle);
    y = y0 + (dx * sinAngle + dy * cosAngle);
}

void Point2D::scale(float kx, float ky, float x0, float y0) {
    x = x0 + (x - x0) * kx;
    y = y0 + (y - y0) * ky;
}