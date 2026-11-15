#ifndef BITMAPHANDLER_HPP
#define BITMAPHANDLER_HPP

#include <SFML/Graphics.hpp>
#include <string>

class BitmapHandler {
public:
    static sf::Image createBitmap(unsigned int width, unsigned int height, sf::Color color = sf::Color::Transparent);
    static bool loadFromFile(sf::Image& image, const std::string& filename);
    static bool saveToFile(const sf::Image& image, const std::string& filename);
    static void copyArea(const sf::Image& source, sf::Image& destination, sf::IntRect sourceRect, sf::Vector2u destPos);
};

#endif
