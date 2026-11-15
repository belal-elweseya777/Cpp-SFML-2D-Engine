#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "SpriteObject.hpp"

enum class PlayerDirection { Down = 0, Left = 1, Right = 2, Up = 3 };

class Player : public SpriteObject, public UpdatableObject {
private:
    float speed = 180.0f;
    PlayerDirection currentDir = PlayerDirection::Down;
    bool isMoving = false;

public:
    Player(float startX, float startY);
    void update(float deltaTime) override;
    void setDirection(PlayerDirection dir);
};

#endif
