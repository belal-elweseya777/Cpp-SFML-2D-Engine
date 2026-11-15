#include "BitmapHandler.hpp"

sf::Image BitmapHandler::createBitmap(unsigned int width, unsigned int height, sf::Color color) {
    sf::Image img;
    img.create(width, height, color);
    return img;
}

bool BitmapHandler::loadFromFile(sf::Image& image, const std::string& filename) {
    return image.loadFromFile(filename);
}

bool BitmapHandler::saveToFile(const sf::Image& image, const std::string& filename) {
    return image.saveToFile(filename);
}

void BitmapHandler::copyArea(const sf::Image& source, sf::Image& destination, sf::IntRect sourceRect, sf::Vector2u destPos) {
    destination.copy(source, destPos.x, destPos.y, sourceRect);
}
