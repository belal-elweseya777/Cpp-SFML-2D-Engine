#ifndef SPRITEOBJECT_HPP
#define SPRITEOBJECT_HPP

#include "GameObject.hpp"

class SpriteObject : public BitmapObject, public AnimatedObject {
protected:
    std::vector<sf::IntRect> animFrames;
    int currentFrame = 0;
    float frameTime = 0.12f;
    float elapsedTime = 0.0f;

public:
    void addFrame(sf::IntRect rect) {
        animFrames.push_back(rect);
        if (animFrames.size() == 1) {
            sprite.setTextureRect(rect);
        }
    }

    void animate(float deltaTime) override {
        if (animFrames.empty()) return;
        elapsedTime += deltaTime;
        if (elapsedTime >= frameTime) {
            elapsedTime = 0.0f;
            currentFrame = (currentFrame + 1) % animFrames.size();
            sprite.setTextureRect(animFrames[currentFrame]);
        }
    }
};

#endif
