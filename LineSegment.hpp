#ifndef LINESEGMENT_HPP
#define LINESEGMENT_HPP

#include "Point2D.hpp"

enum class LineAlgorithm {
    Default,
    Incremental
};

class LineSegment : public ShapeObject {
private:
    Point2D p1;
    Point2D p2;

public:
    LineSegment(Point2D p1 = Point2D(), Point2D p2 = Point2D());

    Point2D getP1() const;
    Point2D getP2() const;

    void setP1(const Point2D& newP1);
    void setP2(const Point2D& newP2);

    void draw(PrimitiveRenderer& renderer) override;
    void draw(PrimitiveRenderer& renderer, LineAlgorithm algo);

    void translate(float tx, float ty) override;
    void rotate(float angleDegrees, float x0 = 0, float y0 = 0) override;
    void scale(float kx, float ky, float x0 = 0, float y0 = 0) override;
};

#endif