#include "LineSegment.hpp"
#include "PrimitiveRenderer.hpp"

LineSegment::LineSegment(Point2D p1, Point2D p2)
    : p1(p1), p2(p2) {}

Point2D LineSegment::getP1() const { return p1; }
Point2D LineSegment::getP2() const { return p2; }

void LineSegment::setP1(const Point2D& newP1) { p1 = newP1; }
void LineSegment::setP2(const Point2D& newP2) { p2 = newP2; }

void LineSegment::draw(PrimitiveRenderer& renderer) {
    draw(renderer, LineAlgorithm::Incremental);
}

void LineSegment::draw(PrimitiveRenderer& renderer, LineAlgorithm algo) {
    if (algo == LineAlgorithm::Incremental) {
        renderer.drawLineIncremental(p1, p2);
    } else {
        renderer.drawLineDefault(p1, p2);
    }
}

void LineSegment::translate(float tx, float ty) {
    p1.translate(tx, ty);
    p2.translate(tx, ty);
}

void LineSegment::rotate(float angleDegrees, float x0, float y0) {
    p1.rotate(angleDegrees, x0, y0);
    p2.rotate(angleDegrees, x0, y0);
}

void LineSegment::scale(float kx, float ky, float x0, float y0) {
    p1.scale(kx, ky, x0, y0);
    p2.scale(kx, ky, x0, y0);
}
