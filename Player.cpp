#include "Player.hpp"

Player::Player(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    sprite.setPosition(position);

    sf::Image img;
    img.create(128, 128, sf::Color::Green);
    texture.loadFromImage(img);
    sprite.setTexture(texture);

    for (int dir = 0; dir < 4; ++dir) {
        for (int frame = 0; frame < 4; ++frame) {
            addFrame(sf::IntRect(frame * 32, dir * 32, 32, 32));
        }
    }
}

void Player::setDirection(PlayerDirection dir) {
    currentDir = dir;
}

void Player::update(float deltaTime) {
    sf::Vector2f movement(0.0f, 0.0f);
    isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        movement.y -= speed * deltaTime;
        setDirection(PlayerDirection::Up);
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        movement.y += speed * deltaTime;
        setDirection(PlayerDirection::Down);
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        movement.x -= speed * deltaTime;
        setDirection(PlayerDirection::Left);
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        movement.x += speed * deltaTime;
        setDirection(PlayerDirection::Right);
        isMoving = true;
    }

    translate(movement.x, movement.y);

    if (isMoving) {
        animate(deltaTime);
    }
}
