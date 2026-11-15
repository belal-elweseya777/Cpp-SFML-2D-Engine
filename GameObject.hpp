#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class PrimitiveRenderer;

class GameObject {
public:
    virtual ~GameObject() = default;
};

class UpdatableObject : virtual public GameObject {
public:
    virtual void update(float deltaTime) = 0;
};

class DrawableObject : virtual public GameObject {
public:
    virtual void draw(PrimitiveRenderer& renderer) = 0;
    virtual void draw(sf::RenderTarget& target) {}
};

class TransformableObject : virtual public GameObject {
public:
    virtual void translate(float tx, float ty) = 0;
    virtual void rotate(float angleDegrees, float x0 = 0, float y0 = 0) = 0;
    virtual void scale(float kx, float ky, float x0 = 0, float y0 = 0) = 0;
};

class ShapeObject : virtual public DrawableObject, virtual public TransformableObject {};

class AnimatedObject : virtual public GameObject {
public:
    virtual void animate(float deltaTime) = 0;
};

class BitmapObject : virtual public DrawableObject, virtual public TransformableObject {
protected:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f position;

public:
    BitmapObject() : position(0.0f, 0.0f) {}

    bool loadTexture(const std::string& filename) {
        if (texture.loadFromFile(filename)) {
            sprite.setTexture(texture);
            return true;
        }
        return false;
    }

    void draw(PrimitiveRenderer& renderer) override {}

    void draw(sf::RenderTarget& target) override {
        target.draw(sprite);
    }

    void translate(float tx, float ty) override {
        position.x += tx;
        position.y += ty;
        sprite.setPosition(position);
    }

    void rotate(float angleDegrees, float x0 = 0, float y0 = 0) override {
        sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
        sprite.setRotation(angleDegrees);
    }

    void scale(float kx, float ky, float x0 = 0, float y0 = 0) override {
        sprite.setScale(kx, ky);
    }

    sf::Vector2f getPosition() const { return position; }
    void setPosition(float x, float y) {
        position = sf::Vector2f(x, y);
        sprite.setPosition(position);
    }
};

#endif