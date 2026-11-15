#include "PrimitiveRenderer.hpp"
#include <cmath>
#include <queue>
#include <algorithm>

PrimitiveRenderer::PrimitiveRenderer(sf::RenderTarget& target)
    : target(target) {}

void PrimitiveRenderer::drawPoint(const Point2D& point) {
    drawPoint(point.getX(), point.getY(), point.getColor());
}

void PrimitiveRenderer::drawPoint(float x, float y, sf::Color color) {
    sf::Vertex vertex(sf::Vector2f(x, y), color);
    target.draw(&vertex, 1, sf::Points);
}

void PrimitiveRenderer::drawLineDefault(const Point2D& p1, const Point2D& p2) {
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(p1.getX(), p1.getY()), p1.getColor()),
        sf::Vertex(sf::Vector2f(p2.getX(), p2.getY()), p2.getColor())
    };
    target.draw(line, 2, sf::Lines);
}

void PrimitiveRenderer::drawLineIncremental(const Point2D& p1, const Point2D& p2) {
    float x1 = p1.getX(), y1 = p1.getY();
    float x2 = p2.getX(), y2 = p2.getY();
    sf::Color color = p1.getColor();

    float dx = x2 - x1;
    float dy = y2 - y1;

    if (std::abs(dx) >= std::abs(dy)) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        float m = (x2 != x1) ? (y2 - y1) / (x2 - x1) : 0.0f;
        float y = y1;
        for (int x = static_cast<int>(std::round(x1)); x <= static_cast<int>(std::round(x2)); ++x) {
            drawPoint(static_cast<float>(x), std::round(y), color);
            y += m;
        }
    } else {
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        float m = (y2 != y1) ? (x2 - x1) / (y2 - y1) : 0.0f;
        float x = x1;
        for (int y = static_cast<int>(std::round(y1)); y <= static_cast<int>(std::round(y2)); ++y) {
            drawPoint(std::round(x), static_cast<float>(y), color);
            x += m;
        }
    }
}

void PrimitiveRenderer::drawCircle(float cx, float cy, float radius, sf::Color color) {
    float x = 0.0f;
    float y = radius;
    float d = 3.0f - 2.0f * radius;

    auto plot8Fold = [&](float px, float py) {
        drawPoint(cx + px, cy + py, color);
        drawPoint(cx - px, cy + py, color);
        drawPoint(cx + px, cy - py, color);
        drawPoint(cx - px, cy - py, color);
        drawPoint(cx + py, cy + px, color);
        drawPoint(cx - py, cy + px, color);
        drawPoint(cx + py, cy - px, color);
        drawPoint(cx - py, cy - px, color);
    };

    plot8Fold(x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4.0f * (x - y) + 10.0f;
        } else {
            d = d + 4.0f * x + 6.0f;
        }
        plot8Fold(x, y);
    }
}

void PrimitiveRenderer::drawEllipse(float cx, float cy, float rx, float ry, sf::Color color) {
    float x = 0.0f;
    float y = ry;

    float rxSq = rx * rx;
    float rySq = ry * ry;

    float d1 = rySq - (rxSq * ry) + (0.25f * rxSq);
    float dx = 2.0f * rySq * x;
    float dy = 2.0f * rxSq * y;

    auto plot4Fold = [&](float px, float py) {
        drawPoint(cx + px, cy + py, color);
        drawPoint(cx - px, cy + py, color);
        drawPoint(cx + px, cy - py, color);
        drawPoint(cx - px, cy - py, color);
    };

    plot4Fold(x, y);

    while (dx < dy) {
        x++;
        dx += 2.0f * rySq;
        if (d1 < 0) {
            d1 += dx + rySq;
        } else {
            y--;
            dy -= 2.0f * rxSq;
            d1 += dx - dy + rySq;
        }
        plot4Fold(x, y);
    }

    float d2 = (rySq * (x + 0.5f) * (x + 0.5f)) + (rxSq * (y - 1.0f) * (y - 1.0f)) - (rxSq * rySq);
    while (y >= 0) {
        y--;
        dy -= 2.0f * rxSq;
        if (d2 > 0) {
            d2 += rxSq - dy;
        } else {
            x++;
            dx += 2.0f * rySq;
            d2 += dx - dy + rxSq;
        }
        plot4Fold(x, y);
    }
}

void PrimitiveRenderer::drawPolyline(const std::vector<Point2D>& points, bool closed) {
    if (points.size() < 2) return;
    for (size_t i = 0; i < points.size() - 1; ++i) {
        drawLineIncremental(points[i], points[i + 1]);
    }
    if (closed) {
        drawLineIncremental(points.back(), points.front());
    }
}

void PrimitiveRenderer::drawPolygon(const std::vector<Point2D>& points) {
    drawPolyline(points, true);
}

void PrimitiveRenderer::boundaryFill(int startX, int startY, sf::Color fillColor, sf::Color borderColor) {
    sf::RenderWindow* window = dynamic_cast<sf::RenderWindow*>(&target);
    if (!window) return;

    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    sf::Image img = texture.copyToImage();

    std::queue<sf::Vector2i> q;
    q.push(sf::Vector2i(startX, startY));

    while (!q.empty()) {
        sf::Vector2i p = q.front();
        q.pop();

        if (p.x < 0 || p.x >= static_cast<int>(img.getSize().x) || p.y < 0 || p.y >= static_cast<int>(img.getSize().y))
            continue;

        sf::Color currentColor = img.getPixel(p.x, p.y);
        if (currentColor != borderColor && currentColor != fillColor) {
            img.setPixel(p.x, p.y, fillColor);
            drawPoint(static_cast<float>(p.x), static_cast<float>(p.y), fillColor);

            q.push(sf::Vector2i(p.x + 1, p.y));
            q.push(sf::Vector2i(p.x - 1, p.y));
            q.push(sf::Vector2i(p.x, p.y + 1));
            q.push(sf::Vector2i(p.x, p.y - 1));
        }
    }
}

void PrimitiveRenderer::floodFill(int startX, int startY, sf::Color fillColor, sf::Color targetColor) {
    sf::RenderWindow* window = dynamic_cast<sf::RenderWindow*>(&target);
    if (!window) return;

    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    sf::Image img = texture.copyToImage();

    if (targetColor == fillColor) return;

    std::queue<sf::Vector2i> q;
    q.push(sf::Vector2i(startX, startY));

    while (!q.empty()) {
        sf::Vector2i p = q.front();
        q.pop();

        if (p.x < 0 || p.x >= static_cast<int>(img.getSize().x) || p.y < 0 || p.y >= static_cast<int>(img.getSize().y))
            continue;

        if (img.getPixel(p.x, p.y) == targetColor) {
            img.setPixel(p.x, p.y, fillColor);
            drawPoint(static_cast<float>(p.x), static_cast<float>(p.y), fillColor);

            q.push(sf::Vector2i(p.x + 1, p.y));
            q.push(sf::Vector2i(p.x - 1, p.y));
            q.push(sf::Vector2i(p.x, p.y + 1));
            q.push(sf::Vector2i(p.x, p.y - 1));
        }
    }
}