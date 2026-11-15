#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include "PrimitiveRenderer.hpp"
#include "Point2D.hpp"
#include "LineSegment.hpp"
#include "Player.hpp"
#include "BitmapHandler.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @class MountedTurret
 * @brief Vector laser cannon mounted on the Cyber Cat, demonstrating centroid rotation and homothety.
 */
class MountedTurret : public ShapeObject {
private:
    Point2D tip, leftBase, rightBase;

public:
    MountedTurret(float x, float y)
        : tip(x, y - 42.0f, sf::Color::Cyan),
          leftBase(x - 10.0f, y - 22.0f, sf::Color(0, 180, 255)),
          rightBase(x + 10.0f, y - 22.0f, sf::Color(0, 180, 255)) {}

    sf::Vector2f getTipPosition() const { return sf::Vector2f(tip.getX(), tip.getY()); }

    sf::Vector2f getAimDirection(float cx, float cy) const {
        float dx = tip.getX() - cx;
        float dy = tip.getY() - cy;
        float len = std::hypot(dx, dy);
        return (len > 0.001f) ? sf::Vector2f(dx / len, dy / len) : sf::Vector2f(0.0f, -1.0f);
    }

    void translate(float tx, float ty) override {
        tip.translate(tx, ty);
        leftBase.translate(tx, ty);
        rightBase.translate(tx, ty);
    }

    void rotate(float angleDegrees, float x0 = 0, float y0 = 0) override {
        tip.rotate(angleDegrees, x0, y0);
        leftBase.rotate(angleDegrees, x0, y0);
        rightBase.rotate(angleDegrees, x0, y0);
    }

    void scale(float kx, float ky, float x0 = 0, float y0 = 0) override {
        tip.scale(kx, ky, x0, y0);
        leftBase.scale(kx, ky, x0, y0);
        rightBase.scale(kx, ky, x0, y0);
    }

    void draw(PrimitiveRenderer& renderer) override {
        renderer.drawLineIncremental(tip, leftBase);
        renderer.drawLineIncremental(leftBase, rightBase);
        renderer.drawLineIncremental(rightBase, tip);
    }
};

struct Star { float x, y, speed; int radius; sf::Color color; };
struct Particle { float x, y, vx, vy, life; sf::Color color; };
struct LaserBolt { float x, y, vx, vy, life; };
struct EnergyOrb { float x, y, vx, vy, pulse; int radius; };
struct Asteroid { float x, y, vx, vy; int radius; int hp; };

// ==================== CUSTOM VECTOR TYPOGRAPHY ENGINE ====================
void drawVectorChar(PrimitiveRenderer& renderer, char ch, float x, float y, float w, float h, sf::Color c) {
    float mx = x + w * 0.5f, my = y + h * 0.5f;
    float r = x + w, b = y + h;
    auto L = [&](float x1, float y1, float x2, float y2) {
        renderer.drawLineIncremental(Point2D(x1, y1, c), Point2D(x2, y2, c));
    };

    switch (ch) {
        case 'G': L(r,y, x,y); L(x,y, x,b); L(x,b, r,b); L(r,b, r,my); L(r,my, mx,my); break;
        case 'A': L(x,b, mx,y); L(mx,y, r,b); L(x+w*0.2f,my+h*0.1f, r-w*0.2f,my+h*0.1f); break;
        case 'M': L(x,b, x,y); L(x,y, mx,my); L(mx,my, r,y); L(r,y, r,b); break;
        case 'E': L(r,y, x,y); L(x,y, x,b); L(x,b, r,b); L(x,my, r-w*0.2f,my); break;
        case 'O': L(x,y, r,y); L(r,y, r,b); L(r,b, x,b); L(x,b, x,y); break;
        case 'V': L(x,y, mx,b); L(mx,b, r,y); break;
        case 'R': L(x,b, x,y); L(x,y, r,y); L(r,y, r,my); L(r,my, x,my); L(mx,my, r,b); break;
        case 'L': L(x,y, x,b); L(x,b, r,b); break;
        case 'D': L(x,y, x,b); L(x,y, r-w*0.2f,y); L(r-w*0.2f,y, r,my); L(r,my, r-w*0.2f,b); L(r-w*0.2f,b, x,b); break;
        case 'P': L(x,b, x,y); L(x,y, r,y); L(r,y, r,my); L(r,my, x,my); break;
        case 'S': L(r,y, x,y); L(x,y, x,my); L(x,my, r,my); L(r,my, r,b); L(r,b, x,b); break;
        case 'C': L(r,y, x,y); L(x,y, x,b); L(x,b, r,b); break;
        case 'H': L(x,y, x,b); L(r,y, r,b); L(x,my, r,my); break;
        case 'T': L(x,y, r,y); L(mx,y, mx,b); break;
        case '[': L(r,y, x,y); L(x,y, x,b); L(x,b, r,b); break;
        case ']': L(x,y, r,y); L(r,y, r,b); L(r,b, x,b); break;
        case ':': L(mx,y+h*0.25f, mx,y+h*0.35f); L(mx,b-h*0.35f, mx,b-h*0.25f); break;
        default: break;
    }
}

void drawVectorText(PrimitiveRenderer& renderer, const std::string& text, float x, float y, float charW, float charH, sf::Color c) {
    float spacing = charW * 1.45f;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] != ' ')
            drawVectorChar(renderer, text[i], x + i * spacing, y, charW, charH, c);
    }
}

void drawVectorDigit(PrimitiveRenderer& renderer, int digit, float x, float y, float size, sf::Color color) {
    static const bool seg[10][7] = {
        {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
        {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };
    if (digit < 0 || digit > 9) return;
    float w = size, h = size * 1.7f, hh = h * 0.5f;
    Point2D tl(x, y, color), tr(x + w, y, color), ml(x, y + hh, color);
    Point2D mr(x + w, y + hh, color), bl(x, y + h, color), br(x + w, y + h, color);

    if (seg[digit][0]) renderer.drawLineIncremental(tl, tr);
    if (seg[digit][1]) renderer.drawLineIncremental(tr, mr);
    if (seg[digit][2]) renderer.drawLineIncremental(mr, br);
    if (seg[digit][3]) renderer.drawLineIncremental(bl, br);
    if (seg[digit][4]) renderer.drawLineIncremental(ml, bl);
    if (seg[digit][5]) renderer.drawLineIncremental(tl, ml);
    if (seg[digit][6]) renderer.drawLineIncremental(ml, mr);
}

void drawVectorNumber(PrimitiveRenderer& renderer, int num, float x, float y, float size, sf::Color color) {
    std::string s = std::to_string(num);
    for (size_t i = 0; i < s.size(); ++i)
        drawVectorDigit(renderer, s[i] - '0', x + i * (size * 1.5f), y, size, color);
}

// ==================== REALISTIC CELESTIAL SYSTEM (SUN + RINGED PLANET + MOON) ====================
void drawCelestialBackdrop(PrimitiveRenderer& renderer, float time) {
    int sunX = 830, sunY = 155;

    // 1. Multi-layered Glowing Sun Corona
    renderer.drawCircle(sunX, sunY, 14, sf::Color(255, 255, 220));
    renderer.drawCircle(sunX, sunY, 22, sf::Color(255, 220, 70));
    renderer.drawCircle(sunX, sunY, 30, sf::Color(255, 140, 30));
    renderer.drawCircle(sunX, sunY, 38, sf::Color(180, 60, 20));

    // Rotating Solar Flare Rays
    for (int i = 0; i < 12; ++i) {
        float ang = time * 0.35f + i * (static_cast<float>(M_PI) / 6.0f);
        float r1 = 42.0f;
        float r2 = (i % 2 == 0) ? 54.0f : 48.0f;
        renderer.drawLineIncremental(
            Point2D(sunX + std::cos(ang) * r1, sunY + std::sin(ang) * r1, sf::Color(255, 170, 40)),
            Point2D(sunX + std::cos(ang) * r2, sunY + std::sin(ang) * r2, sf::Color(140, 50, 15))
        );
    }

    // 2. Elliptical Planetary Orbit Path
    renderer.drawEllipse(sunX, sunY, 130, 58, sf::Color(75, 55, 120));

    // 3. Ringed Gas Giant Planet (Saturn-like)
    float pX = sunX + std::cos(time * 0.75f) * 130.0f;
    float pY = sunY + std::sin(time * 0.75f) * 58.0f;
    int px = static_cast<int>(pX);
    int py = static_cast<int>(pY);

    renderer.drawCircle(px, py, 14, sf::Color(60, 170, 240));
    renderer.drawCircle(px - 3, py - 3, 8, sf::Color(130, 215, 255)); // Atmospheric highlight
    renderer.drawEllipse(px, py, 28, 9, sf::Color(190, 160, 255));     // Outer Planetary Ring
    renderer.drawEllipse(px, py, 23, 7, sf::Color(120, 220, 255));     // Inner Planetary Ring

    // 4. Orbiting Moon around the Planet
    int moonX = px + static_cast<int>(std::cos(time * 3.2f) * 34.0f);
    int moonY = py + static_cast<int>(std::sin(time * 3.2f) * 20.0f);
    renderer.drawCircle(moonX, moonY, 4, sf::Color(210, 215, 230));
}

// ==================== CYBER SPACE CAT AVATAR ====================
void drawCyberCat(PrimitiveRenderer& renderer, float cx, float cy, int facingDir, bool isMoving, float time) {
    int x = static_cast<int>(cx);
    int y = static_cast<int>(cy);
    sf::Color fur(0, 235, 175);
    sf::Color pink(255, 105, 180);
    sf::Color visor(120, 200, 255);

    // Jetpack Thruster Flame when moving
    if (isMoving) {
        int flameLen = 14 + static_cast<int>(std::sin(time * 35.0f) * 6.0f);
        renderer.drawLineIncremental(Point2D(x - 6, y + 22, sf::Color(255, 180, 0)), Point2D(x, y + 22 + flameLen, sf::Color::Red));
        renderer.drawLineIncremental(Point2D(x + 6, y + 22, sf::Color(255, 180, 0)), Point2D(x, y + 22 + flameLen, sf::Color::Red));
    }

    renderer.drawCircle(x, y, 24, visor);
    renderer.drawCircle(x, y + 2, 15, fur);

    // Ears
    renderer.drawLineIncremental(Point2D(x - 13, y - 7, fur), Point2D(x - 17, y - 22, pink));
    renderer.drawLineIncremental(Point2D(x - 17, y - 22, pink), Point2D(x - 4, y - 12, fur));
    renderer.drawLineIncremental(Point2D(x + 13, y - 7, fur), Point2D(x + 17, y - 22, pink));
    renderer.drawLineIncremental(Point2D(x + 17, y - 22, pink), Point2D(x + 4, y - 12, fur));

    // Eyes & Whiskers
    int look = facingDir * 3;
    renderer.drawCircle(x - 5 + look, y, 2, sf::Color::Yellow);
    renderer.drawCircle(x + 5 + look, y, 2, sf::Color::Yellow);
    renderer.drawLineIncremental(Point2D(x - 9, y + 5, sf::Color::White), Point2D(x - 19, y + 4, sf::Color::White));
    renderer.drawLineIncremental(Point2D(x + 9, y + 5, sf::Color::White), Point2D(x + 19, y + 4, sf::Color::White));

    // Tail
    int wag = static_cast<int>(std::sin(time * 9.0f) * 7.0f);
    renderer.drawLineIncremental(Point2D(x, y + 17, fur), Point2D(x - 12 + wag, y + 26, pink));
}

// ==================== GAME OVER & RELOAD [R] OVERLAY ====================
void drawGameOverScreen(PrimitiveRenderer& renderer, float cx, float cy, int finalScore, float time) {
    sf::Color red(255, 60, 80);
    sf::Color gold(255, 215, 60);
    sf::Color cyan(0, 240, 255);

    // Double Neon Arcade Box
    renderer.drawLineIncremental(Point2D(cx - 240, cy - 115, red), Point2D(cx + 240, cy - 115, red));
    renderer.drawLineIncremental(Point2D(cx - 240, cy + 125, red), Point2D(cx + 240, cy + 125, red));
    renderer.drawLineIncremental(Point2D(cx - 240, cy - 115, red), Point2D(cx - 240, cy + 125, red));
    renderer.drawLineIncremental(Point2D(cx + 240, cy - 115, red), Point2D(cx + 240, cy + 125, red));

    renderer.drawLineIncremental(Point2D(cx - 234, cy - 109, gold), Point2D(cx + 234, cy - 109, gold));
    renderer.drawLineIncremental(Point2D(cx - 234, cy + 119, gold), Point2D(cx + 234, cy + 119, gold));
    renderer.drawLineIncremental(Point2D(cx - 234, cy - 109, gold), Point2D(cx - 234, cy + 119, gold));
    renderer.drawLineIncremental(Point2D(cx + 234, cy - 109, gold), Point2D(cx + 234, cy + 119, gold));

    // Title: "GAME OVER"
    drawVectorText(renderer, "GAME OVER", cx - 165.0f, cy - 82.0f, 26.0f, 38.0f, red);

    // Score Display: "SCORE: XXX"
    drawVectorText(renderer, "SCORE:", cx - 135.0f, cy - 15.0f, 15.0f, 24.0f, cyan);
    drawVectorNumber(renderer, finalScore, cx + 15.0f, cy - 15.0f, 14.0f, sf::Color(0, 255, 170));

    // Blinking Reload Prompt Button: "RELOAD [R]"
    bool blink = (static_cast<int>(time * 3.0f) % 2 == 0);
    sf::Color btnColor = blink ? gold : sf::Color(180, 140, 40);

    // Button Border
    renderer.drawLineIncremental(Point2D(cx - 145, cy + 45, btnColor), Point2D(cx + 145, cy + 45, btnColor));
    renderer.drawLineIncremental(Point2D(cx - 145, cy + 92, btnColor), Point2D(cx + 145, cy + 92, btnColor));
    renderer.drawLineIncremental(Point2D(cx - 145, cy + 45, btnColor), Point2D(cx - 145, cy + 92, btnColor));
    renderer.drawLineIncremental(Point2D(cx + 145, cy + 45, btnColor), Point2D(cx + 145, cy + 92, btnColor));

    drawVectorText(renderer, "RELOAD [R]", cx - 122.0f, cy + 56.0f, 17.0f, 24.0f, btnColor);
}

void spawnExplosion(std::vector<Particle>& particles, float x, float y, sf::Color col, int count = 26) {
    for (int i = 0; i < count; ++i) {
        float ang = (std::rand() % 360) * static_cast<float>(M_PI) / 180.0f;
        float spd = 75.0f + (std::rand() % 210);
        particles.push_back({x, y, std::cos(ang) * spd, std::sin(ang) * spd, 0.55f, col});
    }
}

void spawnAsteroid(Asteroid& ast, int width, int height, float catX, float catY) {
    int edge = std::rand() % 4;
    if (edge == 0) { ast.x = static_cast<float>(std::rand() % width); ast.y = -25.0f; }
    else if (edge == 1) { ast.x = static_cast<float>(std::rand() % width); ast.y = height + 25.0f; }
    else if (edge == 2) { ast.x = -25.0f; ast.y = static_cast<float>(std::rand() % height); }
    else { ast.x = width + 25.0f; ast.y = static_cast<float>(std::rand() % height); }

    float dx = (catX + (std::rand() % 150 - 75)) - ast.x;
    float dy = (catY + (std::rand() % 150 - 75)) - ast.y;
    float len = std::hypot(dx, dy);
    float speed = 100.0f + static_cast<float>(std::rand() % 70);
    ast.vx = (dx / len) * speed;
    ast.vy = (dy / len) * speed;
    ast.radius = 18 + (std::rand() % 8);
    ast.hp = 1;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const unsigned int WIDTH = 1024;
    const unsigned int HEIGHT = 720;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cyber Space Cat - 2D Vector Engine & Arcade");
    window.setFramerateLimit(60);

    PrimitiveRenderer renderer(window);
    Player playerEntity(WIDTH / 2.0f, HEIGHT / 2.0f);

    float catX = WIDTH / 2.0f;
    float catY = HEIGHT / 2.0f;
    MountedTurret turret(catX, catY);

    std::vector<Star> stars;
    for (int i = 0; i < 70; ++i) {
        int layer = i % 3;
        stars.push_back({
            static_cast<float>(std::rand() % WIDTH),
            static_cast<float>(std::rand() % HEIGHT),
            20.0f + layer * 30.0f,
            layer == 2 ? 2 : 1,
            layer == 2 ? sf::Color(215, 230, 255) : sf::Color(90, 105, 140)
        });
    }

    std::vector<EnergyOrb> orbs;
    for (int i = 0; i < 4; ++i) {
        orbs.push_back({
            static_cast<float>(120 + std::rand() % (WIDTH - 240)),
            static_cast<float>(120 + std::rand() % (HEIGHT - 240)),
            static_cast<float>((std::rand() % 130) - 65),
            static_cast<float>((std::rand() % 130) - 65),
            static_cast<float>(i), 14
        });
    }

    std::vector<Asteroid> asteroids(8);
    for (auto& ast : asteroids) spawnAsteroid(ast, WIDTH, HEIGHT, catX, catY);

    std::vector<Particle> particles;
    std::vector<LaserBolt> lasers;

    int score = 0;
    int health = 100;
    bool gameOver = false;
    float totalTime = 0.0f;
    float fireCooldown = 0.0f;
    float shakeTimer = 0.0f;
    bool showGrid = true;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) window.close();
                if (event.key.code == sf::Keyboard::G) showGrid = !showGrid;
                if (event.key.code == sf::Keyboard::R) {
                    score = 0;
                    health = 100;
                    gameOver = false;
                    shakeTimer = 0.0f;
                    catX = WIDTH / 2.0f;
                    catY = HEIGHT / 2.0f;
                    turret = MountedTurret(catX, catY);
                    lasers.clear();
                    particles.clear();
                    for (auto& ast : asteroids) spawnAsteroid(ast, WIDTH, HEIGHT, catX, catY);
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        totalTime += deltaTime;
        if (fireCooldown > 0.0f) fireCooldown -= deltaTime;
        if (shakeTimer > 0.0f) shakeTimer -= deltaTime;

        int catFacing = 0;
        bool isMoving = false;

        if (!gameOver) {
            float speed = 255.0f;
            float moveX = 0.0f, moveY = 0.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { moveX -= speed * deltaTime; catFacing = -1; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { moveX += speed * deltaTime; catFacing = 1; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { moveY -= speed * deltaTime; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { moveY += speed * deltaTime; isMoving = true; }

            float nextX = std::max(45.0f, std::min(static_cast<float>(WIDTH - 45), catX + moveX));
            float nextY = std::max(75.0f, std::min(static_cast<float>(HEIGHT - 45), catY + moveY));
            turret.translate(nextX - catX, nextY - catY);
            catX = nextX;
            catY = nextY;
            playerEntity.update(deltaTime);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                turret.rotate(-220.0f * deltaTime, catX, catY);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                turret.rotate(220.0f * deltaTime, catX, catY);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && fireCooldown <= 0.0f) {
                sf::Vector2f tip = turret.getTipPosition();
                sf::Vector2f dir = turret.getAimDirection(catX, catY);
                lasers.push_back({tip.x, tip.y, dir.x * 640.0f, dir.y * 640.0f, 1.3f});
                fireCooldown = 0.15f;
            }

            // Moving Green Energy Orbs
            for (auto& orb : orbs) {
                orb.x += orb.vx * deltaTime;
                orb.y += orb.vy * deltaTime;
                if (orb.x < 60 || orb.x > WIDTH - 60) orb.vx = -orb.vx;
                if (orb.y < 90 || orb.y > HEIGHT - 60) orb.vy = -orb.vy;

                if (std::hypot(catX - orb.x, catY - orb.y) <= orb.radius + 24.0f) {
                    spawnExplosion(particles, orb.x, orb.y, sf::Color(0, 255, 170), 22);
                    score += 20;
                    health = std::min(100, health + 5);
                    orb.x = static_cast<float>(100 + std::rand() % (WIDTH - 200));
                    orb.y = static_cast<float>(110 + std::rand() % (HEIGHT - 210));
                }
            }

            // Swarm of Asteroids
            for (auto& ast : asteroids) {
                ast.x += ast.vx * deltaTime;
                ast.y += ast.vy * deltaTime;

                if (ast.x < -60 || ast.x > WIDTH + 60 || ast.y < -60 || ast.y > HEIGHT + 60)
                    spawnAsteroid(ast, WIDTH, HEIGHT, catX, catY);

                if (std::hypot(ast.x - catX, ast.y - catY) <= ast.radius + 20.0f) {
                    spawnExplosion(particles, ast.x, ast.y, sf::Color::Red, 32);
                    health = std::max(0, health - 20);
                    shakeTimer = 0.25f;
                    spawnAsteroid(ast, WIDTH, HEIGHT, catX, catY);
                    if (health == 0) gameOver = true;
                }
            }

           // Laser Hits
         for (auto& laser : lasers) {
             laser.x += laser.vx * deltaTime;
             laser.y += laser.vy * deltaTime;
             laser.life -= deltaTime;

             for (auto& ast : asteroids) {
                 if (std::hypot(laser.x - ast.x, laser.y - ast.y) <= ast.radius + 8.0f) {
                     laser.life = 0.0f;
                     spawnExplosion(particles, ast.x, ast.y, sf::Color(255, 65, 80), 22);
                     spawnExplosion(particles, ast.x, ast.y, sf::Color(255, 200, 50), 18);
                     score += 10;
                     shakeTimer = 0.14f;
                     spawnAsteroid(ast, WIDTH, HEIGHT, catX, catY);
                 }
             }
            }
            lasers.erase(std::remove_if(lasers.begin(), lasers.end(), [](const LaserBolt& l) { return l.life <= 0.0f; }), lasers.end());
        }

        for (auto& star : stars) {
            star.y += star.speed * deltaTime;
            if (star.y > HEIGHT) { star.y = 0.0f; star.x = static_cast<float>(std::rand() % WIDTH); }
        }
        for (auto& p : particles) {
            p.x += p.vx * deltaTime;
            p.y += p.vy * deltaTime;
            p.life -= deltaTime;
        }
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.life <= 0.0f; }), particles.end());

        // Camera Screen Shake Offset
        sf::View view = window.getDefaultView();
        if (shakeTimer > 0.0f) {
            float offsetX = static_cast<float>((std::rand() % 10) - 5);
            float offsetY = static_cast<float>((std::rand() % 10) - 5);
            view.setCenter(WIDTH / 2.0f + offsetX, HEIGHT / 2.0f + offsetY);
        }
        window.setView(view);

        // ==================== RENDERING ====================
        window.clear(sf::Color(10, 12, 22));

        if (showGrid) {
            sf::Color gridColor(19, 23, 36);
            for (int gx = 0; gx < static_cast<int>(WIDTH); gx += 64)
                renderer.drawLineIncremental(Point2D(gx, 0, gridColor), Point2D(gx, HEIGHT, gridColor));
            for (int gy = 0; gy < static_cast<int>(HEIGHT); gy += 64)
                renderer.drawLineIncremental(Point2D(0, gy, gridColor), Point2D(WIDTH, gy, gridColor));
        }

        for (const auto& star : stars)
            renderer.drawCircle(static_cast<int>(star.x), static_cast<int>(star.y), star.radius, star.color);

        // Realistic Sun + Saturn Ringed Planet + Orbiting Moon
        drawCelestialBackdrop(renderer, totalTime);

        // Green Energy Orbs
        for (auto& orb : orbs) {
            orb.pulse += 4.5f * deltaTime;
            int r = orb.radius + static_cast<int>(std::sin(orb.pulse) * 3.0f);
            renderer.drawCircle(static_cast<int>(orb.x), static_cast<int>(orb.y), r, sf::Color(0, 255, 160));
            renderer.drawCircle(static_cast<int>(orb.x), static_cast<int>(orb.y), r / 2, sf::Color::White);
        }

        // Red Asteroids
        for (const auto& ast : asteroids) {
            renderer.drawCircle(static_cast<int>(ast.x), static_cast<int>(ast.y), ast.radius, sf::Color(255, 55, 70));
            if (ast.hp > 1)
                renderer.drawCircle(static_cast<int>(ast.x), static_cast<int>(ast.y), ast.radius / 2, sf::Color(255, 185, 60));
        }

        for (const auto& laser : lasers) {
            renderer.drawLineIncremental(Point2D(laser.x, laser.y, sf::Color::Red),
                                         Point2D(laser.x - laser.vx * 0.025f, laser.y - laser.vy * 0.025f, sf::Color::Yellow));
        }
        for (const auto& p : particles) {
            renderer.drawCircle(static_cast<int>(p.x), static_cast<int>(p.y), 2, p.color);
        }

        if (!gameOver) {
            drawCyberCat(renderer, catX, catY, catFacing, isMoving, totalTime);
            turret.draw(renderer);
        } else {
            drawGameOverScreen(renderer, WIDTH / 2.0f, HEIGHT / 2.0f, score, totalTime);
        }

        // Top & Bottom Arcade HUD Borders
        renderer.drawLineIncremental(Point2D(20, 20, sf::Color::Cyan), Point2D(WIDTH - 20, 20, sf::Color::Cyan));
        renderer.drawLineIncremental(Point2D(20, HEIGHT - 20, sf::Color::Cyan), Point2D(WIDTH - 20, HEIGHT - 20, sf::Color::Cyan));

        // Top-Left: "SCORE:" + Numeric Value
        drawVectorText(renderer, "SCORE:", 34.0f, 35.0f, 11.0f, 18.0f, sf::Color::Cyan);
        drawVectorNumber(renderer, score, 136.0f, 34.0f, 11.0f, sf::Color(0, 255, 170));

        // Top-Right: "HP" + Health Bar
        drawVectorText(renderer, "HP", WIDTH - 285.0f, 35.0f, 11.0f, 18.0f, sf::Color(255, 90, 110));
        if (health > 0) {
            int barWidth = health * 2;
            for (int hy = 35; hy <= 51; hy += 2) {
                renderer.drawLineIncremental(Point2D(WIDTH - 240, hy, sf::Color(255, 65, 85)),
                                             Point2D(WIDTH - 240 + barWidth, hy, sf::Color(255, 65, 85)));
            }
        }

        window.display();
    }

    return 0;
}