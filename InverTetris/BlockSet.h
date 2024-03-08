#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>


typedef std::vector<std::vector<bool>> Shape;

class BlockSet : public sf::Drawable, public sf::Transformable
{

public:

	BlockSet(sf::Texture& texture, sf::Color color, Shape shape);
	BlockSet(sf::Texture& texture);
	void setup(sf::Texture& texture, sf::Color color, Shape shape);
	void removeBlocksAt(int yLevel);
	int getWidth() const;
	int getHeight() const;
	bool partlyBroken() const;
	void rotateShape();
	sf::Color getColor() const;
	void setColor(sf::Color color);
	void addBlock(sf::Vector2f position);

	std::vector<sf::Vector2f> getBlockPositions() const;

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	std::vector<sf::Sprite> mTileSprites;
	Shape mShape;

	sf::Texture* mTexture;
	sf::Color mColor;


	int mWidth;
	int mHeight;
	bool mPartlyBroken;
};
