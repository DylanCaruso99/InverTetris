#include "BlockSet.h"

#include <SFML/Graphics/RenderTarget.hpp>


BlockSet::BlockSet(sf::Texture& texture, sf::Color color, Shape shape)
	: mWidth(shape.size())
	, mHeight(shape[0].size())
	, mTexture(&texture)
	, mColor(color)
	, mPartlyBroken(false)
{
	mTexture = &texture;

	sf::Sprite sprite(texture);
	sprite.setColor(color);

	for (int i = 0; i < shape.size(); i++)
	{
		for (int j = 0; j < shape[0].size(); j++)
		{
			if (shape[i][j])
			{
				sprite.setPosition(32 * i, 32 * j);
				mTileSprites.push_back(sprite);
			}
		}
	}

	mShape = shape;
}

BlockSet::BlockSet(sf::Texture& texture)
	: mTexture(&texture)
	, mPartlyBroken(false)
{
}

bool BlockSet::partlyBroken() const
{
	return mPartlyBroken;
}

sf::Color BlockSet::getColor() const
{
	return mColor;
}

void BlockSet::setColor(sf::Color color)
{
	for (int i = 0; i < mTileSprites.size(); i++)
	{
		mTileSprites[i].setColor(color);
	}

	mColor = color;
}

void BlockSet::addBlock(sf::Vector2f position)
{
	sf::Sprite sprite(*mTexture);
	sprite.setColor(getColor());
	sprite.setPosition(position.x, position.y);
	mTileSprites.push_back(sprite);
}

void BlockSet::setup(sf::Texture& texture, sf::Color color, Shape shape)
{
	mTexture = &texture;
	mTileSprites.clear();
	mColor = color;

	sf::Sprite sprite(texture);
	sprite.setColor(color);

	for (int i = 0; i < shape.size(); i++)
	{
		for (int j = 0; j < shape[0].size(); j++)
		{
			if (shape[i][j])
			{
				sprite.setPosition(32 * i, 32 * j);
				mTileSprites.push_back(sprite);
			}
		}
	}

	mShape = shape;

	mWidth = shape.size();
	mHeight = shape[0].size();
}

std::vector<sf::Vector2f> BlockSet::getBlockPositions() const
{
	std::vector<sf::Vector2f> vec;

	for (int i = 0; i < mTileSprites.size(); i++)
	{
		vec.push_back(sf::Vector2f(mTileSprites[i].getPosition().x + getPosition().x,
			mTileSprites[i].getPosition().y + getPosition().y));
	}

	return vec;
}

void BlockSet::removeBlocksAt(int yLevel)
{
	std::vector<int> toRemove;

	for (int i = 0; i < mTileSprites.size(); i++)
	{
		if ((mTileSprites[i].getPosition().y + getPosition().y) / 32 == yLevel)
		{
			toRemove.push_back(i);
			mPartlyBroken = true;
		}
	}
	for (int i = toRemove.size() - 1; i >= 0; i--)
	{
		mTileSprites.erase(mTileSprites.begin() + toRemove[i]);
	}
}

void BlockSet::rotateShape()
{
	Shape newShape;

	for (int i = 0; i < mShape[0].size(); i++)
	{
		newShape.push_back(std::vector<bool>());
		for (int j = 0; j < mShape.size(); j++)
		{
			newShape[i].push_back(0);
		}
	}
	for (int i = 0; i < mShape[0].size(); i++)
	{
		for (int j = 0; j < mShape.size(); j++)
		{
			newShape[i][j] = mShape[mShape.size() - 1 - j][i];
		}
	}

	setup(*mTexture, mColor, newShape);
}

int BlockSet::getWidth() const
{
	return mWidth;
}

int BlockSet::getHeight() const
{
	return mHeight;
}

void BlockSet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	for (int i = 0; i < mTileSprites.size(); i++)
	{
		target.draw(mTileSprites[i], states);
	}
}
