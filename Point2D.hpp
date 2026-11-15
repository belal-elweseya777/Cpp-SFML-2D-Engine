#ifndef POINT2D_HPP
#define POINT2D_HPP

#include "GameObject.hpp"

class Point2D : public ShapeObject {
private:
    float x;
    float y;
    sf::Color color;

public:
    Point2D(float x = 0.0f, float y = 0.0f, sf::Color color = sf::Color::White);

    float getX() const;
    float getY() const;
    sf::Color getColor() const;

    void setX(float newX);
    void setY(float newY);
    void setColor(sf::Color newColor);

    void draw(PrimitiveRenderer& renderer) override;
    void translate(float tx, float ty) override;
    void rotate(float angleDegrees, float x0 = 0, float y0 = 0) override;
    void scale(float kx, float ky, float x0 = 0, float y0 = 0) override;
};

#endif
