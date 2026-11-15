#ifndef PRIMITIVERENDERER_HPP
#define PRIMITIVERENDERER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Point2D.hpp"

class PrimitiveRenderer {
private:
    sf::RenderTarget& target;

public:
    PrimitiveRenderer(sf::RenderTarget& target);

    void drawPoint(const Point2D& point);
    void drawPoint(float x, float y, sf::Color color = sf::Color::White);

    void drawLineDefault(const Point2D& p1, const Point2D& p2);
    void drawLineIncremental(const Point2D& p1, const Point2D& p2);

    void drawCircle(float cx, float cy, float radius, sf::Color color = sf::Color::White);
    void drawEllipse(float cx, float cy, float rx, float ry, sf::Color color = sf::Color::White);

    void drawPolyline(const std::vector<Point2D>& points, bool closed = false);
    void drawPolygon(const std::vector<Point2D>& points);

    void boundaryFill(int startX, int startY, sf::Color fillColor, sf::Color borderColor);
    void floodFill(int startX, int startY, sf::Color fillColor, sf::Color targetColor);
};

#endif