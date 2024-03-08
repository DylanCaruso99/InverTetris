#pragma once

#include "BlockSet.h"

#include <SFML/Graphics.hpp>


class Game
{

public:

	Game();
	void run();

private:

	enum Status
	{
		Waiting,
		Paused,
		Playing,
		GameOver,
	};

	typedef std::pair<int, std::string> highscore;
	typedef std::pair<sf::Color, Shape> SetType;

	Shape stringToShape(int width, int height, std::string s);
	void addSetTypes();
	void addSetType(sf::Color color, Shape);
	void addHighscores();
	void processInput();
	void render();
	void applyBlurToTexture();
	void updateVertices();
	void update();
	void positionNewSets();
	void newBlockSet();
	void randomizeNextFlip();
	void positionNextFlipCounter();
	bool blockCollision() const;
	bool blockCollision(BlockSet& blockSet) const;
	bool checkFullHorizons(int scoreMultiplier);
	void dropBlockSets();
	void updateTileSheet();
	void updateTileSheet(int exclusion); //Exclude falling blockSets.
	void removeBlockSets();
	void removeBlocksAt(int yLevel);
	void positionGuide();

	sf::RenderWindow  mWindow;
	sf::Font mFont;
	sf::Texture mTileTexture;

	sf::Text mScoreText;
	sf::Text mNextText;
	sf::Text mNextFlipTitleText;
	sf::Text mNextFlipCounterText;

	sf::RectangleShape mScoreRect; //Rect around score.
	sf::RectangleShape mNextRect; //Rect around next shape.
	sf::RectangleShape mGameRect; //Rect that tiles are put onto.
	sf::RectangleShape mNextFlipRect; //Rect around indicator for next gravity flip.
	sf::RectangleShape mBloomGameRect;
	sf::Text mHighscoreTitleText;
	sf::RectangleShape mHighscoresRect;
	sf::Clock mClock;
	sf::Time mTimeSinceLastDrop = sf::Time::Zero;
	sf::Time mFallElapsedTime; //Multiplier to simulate gravity.
	sf::Time mMoveTimer; //Timer for moving with keyboard.
	sf::Time mRotateTimer; //Timer for rotating.
	sf::Time mKeyFallTimer; //Timer for falling (by keyboard).
	sf::Text mDisplayText;

	BlockSet mCurrentBlockSet;
	BlockSet mNextBlockSet;
	BlockSet mGuideBlockSet;
	std::vector<BlockSet> mBlockSets;
	std::vector<SetType> mSetTypes;
	std::vector<sf::Text> mHighscoreTexts;
	std::vector<sf::RectangleShape> mFadeEffects;
	Shape mTileSheet; //2D array of each tile spot.
	int mScore;
	int mNextFlipCounter;
	const int mGameWidth;
	const int mGameHeight;

	std::string username; //For highscores.

	//For the falling block effect:
	std::vector<sf::Vector2f> mMinMaxBlockSetPositions;
	std::vector<sf::Vector2f> mPreviousBlockSetPositions;
	bool mFallEffect;

	//Rendering:
	sf::RenderTexture mSceneTexture;
	sf::RenderTexture mBlurTexture;
	sf::Sprite mRenderSprite;
	sf::RenderStates mBlurStates;
	sf::Shader mBlurShader;

	Status mStatus;
	bool mInverted;
};
