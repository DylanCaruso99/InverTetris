#pragma once

#include <SFML/Graphics.hpp>


class Shape : public sf::Drawable
{

public:

	Shape(sf::Texture texture, std::vector<bool> shape, sf::Color color);

private:

	std::vector<sf::Sprite> mTileSprites;
	sf::VertexArray mTileSheet;
};
