#include "Game.h"

#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <random>


Game::Game()
	: mWindow(sf::VideoMode(484, 644), "Tetris", sf::Style::Close)
	, mGameWidth(10)
	, mGameHeight(20)
	, mScore(0)
	, mCurrentBlockSet(mTileTexture)
	, mNextBlockSet(mTileTexture)
	, mTileSheet()
	, mDisplayText()
	, mStatus(Status::Waiting)
	, mInverted(false)
	, mFallEffect(false)
	, mGuideBlockSet(mTileTexture)
{
	mWindow.setKeyRepeatEnabled(false);
	mSceneTexture.create(484, 644);
	mBlurTexture.create(484, 644);
	mBlurStates.blendMode = sf::BlendAdd;

	mBlurShader.loadFromFile("Assets/Shaders/Blur.frag", sf::Shader::Type::Fragment);
	mWindow.setVerticalSyncEnabled(true);
	srand(time(NULL));

	mFont.loadFromFile("Assets/Fonts/visitor1.ttf");
	mTileTexture.loadFromFile("Assets/Textures/tile.png");

	mGameRect.setFillColor(sf::Color(0, 0, 20));
	mGameRect.setSize(sf::Vector2f(320.f, 640.f));
	mGameRect.setPosition(2.f, 2.f);
	mGameRect.setOutlineColor(sf::Color::White);
	mGameRect.setOutlineThickness(2.f);

	mDisplayText.setFont(mFont);
	mDisplayText.setCharacterSize(20);
	mDisplayText.setFillColor(sf::Color::Red);
	mDisplayText.setPosition(25.f, 300.f);
	mDisplayText.setOutlineColor(sf::Color::Black);
	mDisplayText.setOutlineThickness(2.f);

	mScoreText.setFont(mFont);
	mScoreText.setCharacterSize(20);
	mScoreText.setFillColor(sf::Color::Blue);
	mScoreText.setStyle(sf::Text::Underlined);
	mScoreText.setPosition(328.f, 5.f);

	mScoreRect.setSize(sf::Vector2f(158.f, 46.f));
	mScoreRect.setOutlineColor(sf::Color::White);
	mScoreRect.setOutlineThickness(2.f);
	mScoreRect.setFillColor(sf::Color::Transparent);
	mScoreRect.setPosition(324.f, 2.f);

	mNextText.setFont(mFont);
	mNextText.setCharacterSize(20);
	//mNextText.setOutlineColor(sf::Color::White);
	mNextText.setPosition(328.f, 50.f);
	mNextText.setString("Next Shape:");

	mNextRect.setFillColor(sf::Color::Transparent);
	mNextRect.setSize(sf::Vector2f(158.f, 170.f));
	mNextRect.setOutlineColor(sf::Color::White);
	mNextRect.setOutlineThickness(2.f);
	mNextRect.setPosition(324.f, 50.f);

	mNextFlipTitleText.setFont(mFont);
	mNextFlipTitleText.setCharacterSize(15);
	mNextFlipTitleText.setFillColor(sf::Color::Cyan);
	mNextFlipTitleText.setPosition(328.f, 224.f);
	mNextFlipTitleText.setString("Next Flip After:");

	mNextFlipCounterText.setFont(mFont);
	mNextFlipCounterText.setCharacterSize(120);
	mNextFlipCounterText.setFillColor(sf::Color::Red);
	randomizeNextFlip();

	mNextFlipRect.setFillColor(sf::Color::Transparent);
	mNextFlipRect.setSize(sf::Vector2f(158.f, 170.f));
	mNextFlipRect.setOutlineColor(sf::Color::White);
	mNextFlipRect.setOutlineThickness(2.f);
	mNextFlipRect.setPosition(324.f, 222.f);

	mHighscoreTitleText.setFont(mFont);
	mHighscoreTitleText.setString("Highscores");
	mHighscoreTitleText.setCharacterSize(20);
	mHighscoreTitleText.setFillColor(sf::Color::Blue);
	mHighscoreTitleText.setPosition(328.f, 390.f);

	mHighscoresRect.setSize(sf::Vector2f(158.f, 248.f));
	mHighscoresRect.setFillColor(sf::Color::Transparent);
	mHighscoresRect.setOutlineColor(sf::Color::White);
	mHighscoresRect.setOutlineThickness(2.f);
	mHighscoresRect.setPosition(324.f, 394.f);

	mBloomGameRect = mGameRect;
	mBloomGameRect.setOutlineThickness(0.f);
	mGameRect.setFillColor(sf::Color::Transparent);

	mScoreText.setString("Score: " + std::to_string(mScore));

	updateTileSheet();

	addSetTypes();
	addHighscores();

	SetType type = mSetTypes[rand() % mSetTypes.size()];
	mCurrentBlockSet.setup(mTileTexture, type.first, type.second);

	mGuideBlockSet.setup(mTileTexture, type.first, type.second);
	mGuideBlockSet.setColor(sf::Color(255, 255, 255, 60));

	type = mSetTypes[rand() % mSetTypes.size()];
	mNextBlockSet.setup(mTileTexture, type.first, type.second);

	positionNewSets();
}

void Game::run()
{
	while (mWindow.isOpen())
	{
		processInput();
		update();
		render();
	}
}

Shape Game::stringToShape(int width, int height, std::string s)
{
	Shape shape;

	for (int i = 0; i < width; i++)
	{
		shape.push_back(std::vector<bool>());

		for (int j = 0; j < height; j++)
		{
			shape[i].push_back(s[j * width + i] == '1');
		}
	}

	return shape;
}

void Game::addSetTypes()
{
	//As an example, this adds the "Line" shape
	std::string s = "1111";
	addSetType(sf::Color(160, 226, 255), stringToShape(4, 1, s));

	s = "100";
	s += "111";
	addSetType(sf::Color(0, 0, 255), stringToShape(3, 2, s));

	s = "001";
	s += "111";
	addSetType(sf::Color(255, 140, 0), stringToShape(3, 2, s));

	s = "11";
	s += "11";
	addSetType(sf::Color(204, 204, 0), stringToShape(2, 2, s));

	s = "011";
	s += "110";
	addSetType(sf::Color::Green, stringToShape(3, 2, s));

	s = "010";
	s += "111";
	s += "000"; //Extra row so rotation happens in center.
	addSetType(sf::Color(255, 0, 255), stringToShape(3, 3, s));

	s = "110";
	s += "011";
	addSetType(sf::Color(255, 0, 0), stringToShape(3, 2, s));
}

void Game::addSetType(sf::Color color, Shape shape)
{
	SetType setType(color, shape);
	mSetTypes.push_back(setType);
}

void Game::addHighscores()
//Read the highscores from a file and add them
{
	std::vector<highscore> highscores;
	std::ifstream iFile("highscores.txt");
	std::string name;
	int score;

	while (iFile >> name >> score)
	{
		highscores.push_back(highscore(score, name));
	}

	//Sort highscores:
	std::sort(highscores.begin(), highscores.end());

	//Add highscore texts:
	for (int i = highscores.size() - 1; i >= 0; i--)
	{
		sf::Text text;
		text.setFont(mFont);
		text.setCharacterSize(14);
		text.setFillColor(sf::Color::Magenta);
		text.setString(highscores[i].second + ": " + std::to_string(highscores[i].first));
		text.setPosition(328.f, 396.f + ((highscores.size() - i) * 15.f));
		mHighscoreTexts.push_back(text);
	}

	iFile.close();
}

void Game::positionNewSets()
{
	mCurrentBlockSet.setPosition(160 - (mCurrentBlockSet.getWidth() / 2 * 32), (640 - (mCurrentBlockSet.getHeight() * 32)) * mInverted);
	mNextBlockSet.setPosition(401.f - (mNextBlockSet.getWidth() * 16.f), 133.f - (mNextBlockSet.getHeight() * 16.f));
}

void Game::newBlockSet()
{
	mBlockSets.push_back(mCurrentBlockSet);
	mCurrentBlockSet = mNextBlockSet;
	mGuideBlockSet = mNextBlockSet;
	mGuideBlockSet.setColor(sf::Color(255, 255, 255, 60));
	SetType type = mSetTypes[rand() % mSetTypes.size()];
	mNextBlockSet.setup(mTileTexture, type.first, type.second);
	positionNewSets();
	updateTileSheet();
	checkFullHorizons(1);

	if (blockCollision() && !mFallEffect)
	{
		mStatus = Status::GameOver;
		mWindow.setKeyRepeatEnabled(true);
	}

	mNextFlipCounter--;
	mNextFlipCounterText.setString(std::to_string(mNextFlipCounter));
	positionNextFlipCounter();
}

void Game::randomizeNextFlip()
{
	mNextFlipCounter = std::rand() % 6 + 1;
	mNextFlipCounterText.setString(std::to_string(mNextFlipCounter));
	positionNextFlipCounter();
}

void Game::positionNextFlipCounter()
{
	mNextFlipCounterText.setPosition(401.f - (mNextFlipCounterText.getGlobalBounds().width / 2.f), 210.f);
}

bool Game::checkFullHorizons(int scoreMultiplier)
//Check for full horizontal sets
{
	std::vector<int> fullHorizons;
	//Pushing back N means there is a full horizon on the Nth level

	bool foundEmptySpace = false;

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (!mTileSheet[j][i])
			{
				foundEmptySpace = true;
				break;
			}
		}
		if (!foundEmptySpace)
		{
			fullHorizons.push_back(i);
			//Full horizon found
		}

		foundEmptySpace = false;
	}
	if (fullHorizons.size() != 0)
	{
		if (fullHorizons.size() == 1)
		{
			mScore += 100 * scoreMultiplier;
		}
		else if (fullHorizons.size() == 2)
		{
			mScore += 220 * scoreMultiplier;
		}
		else if (fullHorizons.size() == 3)
		{
			mScore += 360 * scoreMultiplier;
		}
		else
		{
			mScore += 550 * scoreMultiplier;
		}

		mScoreText.setString("Score: " + std::to_string(mScore));
	}
	//Remove blocks on full horizons
	for (int i = 0; i < fullHorizons.size(); i++)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(-320.f, 32.f));
		rect.setPosition(320.f, fullHorizons[i] * 32);

		mFadeEffects.push_back(rect);

		removeBlocksAt(fullHorizons[i]);
	}
	if (fullHorizons.size() != 0)
	{
		updateTileSheet();
		removeBlockSets();
	}

	return (fullHorizons.size() != 0);
}

void Game::removeBlocksAt(int yLevel)
{
	for (int i = 0; i < mBlockSets.size(); i++)
	{
		mBlockSets[i].removeBlocksAt(yLevel);
	}

	std::vector<int> toRemove;

	for (int i = 0; i < mBlockSets.size(); i++)
	{
		if (mBlockSets[i].partlyBroken())
		{
			toRemove.push_back(i);

			Shape shape(1, std::vector<bool>(1, 0));
			//We are not concerned with height/width
			//as we are not going to rotate this shape
			//at any point in time

			BlockSet top(mTileTexture, mBlockSets[i].getColor(), shape);
			BlockSet bottom(mTileTexture, mBlockSets[i].getColor(), shape);

			std::vector<sf::Vector2f> positions = mBlockSets[i].getBlockPositions();

			for (int i = 0; i < positions.size(); i++)
			{
				if (positions[i].y > yLevel * 32)
				{
					bottom.addBlock(positions[i]);
				}
				else if (positions[i].y < yLevel * 32)
				{
					top.addBlock(positions[i]);
				}
			}

			mBlockSets.push_back(top);
			mBlockSets.push_back(bottom);
		}
	}

	std::sort(toRemove.begin(), toRemove.end());

	for (int i = toRemove.size() - 1; i >= 0; i--)
	{
		mBlockSets.erase(mBlockSets.begin() + toRemove[i]);
	}
}

void Game::removeBlockSets()
{
	for (int i = 0; i < mBlockSets.size(); i++)
	{
		if (mBlockSets[i].getBlockPositions().size() == 0)
		{
			mBlockSets.erase(mBlockSets.begin() + i);
			removeBlockSets();
			return;
		}
	}
}

void Game::dropBlockSets()
{
	//Get previous positions of
	//each block set
	for (int i = 0; i < mBlockSets.size(); i++)
	{
		mPreviousBlockSetPositions.push_back(mBlockSets[i].getPosition());
	}

	bool movementFound = true;

	while (movementFound)
	{
		movementFound = false;

		for (int j = 0; j < mBlockSets.size(); j++)
		{
			mBlockSets[j].move(0.f, 32.f - (64.f * mInverted));
			updateTileSheet(j);

			if (blockCollision(mBlockSets[j]))
			{
				mBlockSets[j].move(0.f, -32.f + (64.f * mInverted));
			}
			else
			{
				movementFound = true;
			}

			updateTileSheet();
		}
	}
	//Record the max/min
	//positions that a
	//block falls onto
	for (int i = 0; i < mBlockSets.size(); i++)
	{
		mMinMaxBlockSetPositions.push_back(mBlockSets[i].getPosition());
		mBlockSets[i].setPosition(mPreviousBlockSetPositions[i]);
	}

	mFallEffect = true;
}

void Game::updateTileSheet()
{
	mTileSheet = Shape(10, std::vector<bool>(20, 0));

	for (BlockSet& blockSet : mBlockSets)
	{
		std::vector<sf::Vector2f> positions = blockSet.getBlockPositions();

		for (sf::Vector2f pos : positions)
		{
			mTileSheet[pos.x / 32][pos.y / 32] = true;
		}
	}
}

void Game::updateTileSheet(int exclusion)
{
	mTileSheet = Shape(10, std::vector<bool>(20, 0));

	for (int i = 0; i < mBlockSets.size(); i++)
	{
		if (i == exclusion)
		{
			continue;
		}

		std::vector<sf::Vector2f> positions = mBlockSets[i].getBlockPositions();

		for (sf::Vector2f pos : positions)
		{
			mTileSheet[pos.x / 32][pos.y / 32] = true;
		}
	}
}

bool Game::blockCollision() const
{
	std::vector<sf::Vector2f> positions = mCurrentBlockSet.getBlockPositions();

	if (positions.size() == 0)
	{
		return false;
	}
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i].x >= 320.f ||
			positions[i].x < 0.f ||
			positions[i].y >= 640.f ||
			positions[i].y < 0.f ||
			mTileSheet[positions[i].x / 32][positions[i].y / 32])
		{
			return true;
		}
	}

	return false;
}

bool Game::blockCollision(BlockSet& blockSet) const
{
	std::vector<sf::Vector2f> positions = blockSet.getBlockPositions();

	if (positions.size() == 0)
	{
		return false;
	}
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i].x >= 320.f ||
			positions[i].x < 0.f ||
			positions[i].y >= 640.f ||
			positions[i].y < 0.f ||
			mTileSheet[positions[i].x / 32][positions[i].y / 32])
		{
			return true;
		}
	}

	return false;
}

void Game::positionGuide()
{
	mGuideBlockSet.setPosition(mCurrentBlockSet.getPosition());

	while (true)
	{
		mGuideBlockSet.move(0.f, 32.f - (64.f * mInverted));

		if (blockCollision(mGuideBlockSet))
		{
			mGuideBlockSet.move(0.f, -32.f + (64.f * mInverted));
			break;
		}
	}
}

void Game::processInput()
{
	sf::Event event;

	while (mWindow.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			mWindow.close();
		}
		else if (event.type == sf::Event::KeyPressed && !mFallEffect)
		{
			if (event.key.code == sf::Keyboard::Return && mStatus == Status::Waiting)
			{
				mStatus = Status::Playing;
				mInverted = false;
				mClock.restart();
				mWindow.setKeyRepeatEnabled(false);
			}
			else if (event.key.code == sf::Keyboard::Escape)
			{
				if (mStatus == Status::Playing)
				{
					mStatus = Status::Paused;
				}
				else if (mStatus == Status::Paused)
				{
					mClock.restart();
					mStatus = Status::Playing;
				}
			}
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Left)
			{
				mMoveTimer = sf::Time::Zero;
			}
			else if ((event.key.code == sf::Keyboard::Up && !mInverted) || event.key.code == sf::Keyboard::Down)
			{
				mRotateTimer = sf::Time::Zero;
			}
			else if ((event.key.code == sf::Keyboard::Up && mInverted) || event.key.code == sf::Keyboard::Up)
			{
				mKeyFallTimer = sf::Time::Zero;
			}
		}
		else if (event.type == sf::Event::TextEntered && mStatus == Status::GameOver)
		{
			if (event.text.unicode == '\b')
			{
				if (username.size() != 0)
				{
					username.pop_back();
				}
			}
			else if (event.text.unicode == 13)
			{
				if (username != "")
				{
					mHighscoreTexts.clear();
					std::fstream oFile;
					oFile.open("highscores.txt", std::fstream::in | std::fstream::out | std::fstream::app);
					oFile << username << " " << mScore << std::endl;
					oFile.close();
					addHighscores();
					username = "";
				}

				mScore = 0;
				mScoreText.setString("Score: " + std::to_string(mScore));
				mInverted = false;
				randomizeNextFlip();
				mBlockSets.clear();
				SetType type = mSetTypes[rand() % mSetTypes.size()];
				mCurrentBlockSet.setup(mTileTexture, type.first, type.second);
				mGuideBlockSet.setup(mTileTexture, type.first, type.second);
				mGuideBlockSet.setColor(sf::Color(255, 255, 255, 60));
				type = mSetTypes[rand() % mSetTypes.size()];
				mNextBlockSet.setup(mTileTexture, type.first, type.second);

				positionNewSets();
				updateTileSheet();


				mStatus = Status::Waiting;
			}
			else if (event.text.unicode < 128 && event.text.unicode != 32 && username.size() < 10)
			{
				username.push_back(static_cast<char>(event.text.unicode));
			}
		}
	}
	if (mStatus == Status::Playing && !mFadeEffects.size() && !mFallEffect)
	{
		const sf::Time moveTime = sf::seconds(0.15f);
		const sf::Time rotateTime = sf::seconds(0.6f);
		const sf::Time keyFallTime = sf::seconds(0.05f);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && mMoveTimer <= sf::Time::Zero)
		{
			mMoveTimer = moveTime;
			mCurrentBlockSet.move(32.f, 0.f);

			if (blockCollision())
			{
				mCurrentBlockSet.move(-32.f, 0.f);
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && mMoveTimer <= sf::Time::Zero)
		{
			mMoveTimer = moveTime;
			mCurrentBlockSet.move(-32.f, 0.f);

			if (blockCollision())
			{
				mCurrentBlockSet.move(32.f, 0.f);
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (mInverted && mRotateTimer <= sf::Time::Zero)
			{
				mRotateTimer = rotateTime;
				mCurrentBlockSet.rotateShape();
				mGuideBlockSet.rotateShape();

				if (blockCollision())
				{
					for (int i = 0; i < 3; i++)
					{
						mCurrentBlockSet.rotateShape();
						mGuideBlockSet.rotateShape();
					}
				}
			}
			else if (!mInverted && mKeyFallTimer <= sf::Time::Zero)
			{
				mKeyFallTimer = keyFallTime;
				mCurrentBlockSet.move(0.f, 32.f);

				if (blockCollision())
				{
					mCurrentBlockSet.move(0.f, -32.f);
					newBlockSet();
				}
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (mInverted && mKeyFallTimer <= sf::Time::Zero)
			{
				mKeyFallTimer = keyFallTime;
				mCurrentBlockSet.move(0.f, -32.f);

				if (blockCollision())
				{
					mCurrentBlockSet.move(0.f, 32.f);
					newBlockSet();
				}
			}
			else if (!mInverted && mRotateTimer <= sf::Time::Zero)
			{
				mRotateTimer = rotateTime;
				mCurrentBlockSet.rotateShape();
				mGuideBlockSet.rotateShape();

				if (blockCollision())
				{
					for (int i = 0; i < 3; i++)
					{
						mCurrentBlockSet.rotateShape();
						mGuideBlockSet.rotateShape();
					}
				}
			}
		}
	}
}

void Game::render()
{
	mWindow.clear();
	mSceneTexture.clear();
	mBlurTexture.clear();
	mWindow.draw(mGameRect);

	//For drawing the blocks
	//we offset by 2, to fit
	//past the white outline
	sf::View view = mSceneTexture.getDefaultView();
	view.move(-2.f, -2.f);
	mSceneTexture.setView(view);
	mBlurTexture.setView(view);

	mSceneTexture.draw(mBloomGameRect);

	if (!mFallEffect && !mFadeEffects.size())
	{
		mSceneTexture.draw(mGuideBlockSet);
		mSceneTexture.draw(mCurrentBlockSet);
	}
	for (BlockSet& blockSet : mBlockSets)
	{
		mSceneTexture.draw(blockSet);
	}
	for (sf::RectangleShape& rect : mFadeEffects)
	{
		mSceneTexture.draw(rect);
	}
	for (sf::Text& text : mHighscoreTexts)
	{
		mSceneTexture.draw(text);
	}

	mSceneTexture.draw(mNextBlockSet);
	mSceneTexture.draw(mScoreText);
	mSceneTexture.draw(mNextText);
	mSceneTexture.draw(mNextFlipTitleText);
	mSceneTexture.draw(mHighscoreTitleText);
	mSceneTexture.draw(mNextFlipCounterText);

	if (mStatus != Status::Playing)
	{
		mSceneTexture.draw(mDisplayText);
	}

	mSceneTexture.setView(mSceneTexture.getDefaultView());
	mBlurTexture.setView(mBlurTexture.getDefaultView());

	mSceneTexture.display();

	mRenderSprite.setTexture(mSceneTexture.getTexture());
	mWindow.draw(mRenderSprite);

	applyBlurToTexture();

	mWindow.draw(mRenderSprite, mBlurStates);


	mWindow.draw(mGameRect);
	mWindow.draw(mScoreRect);
	mWindow.draw(mNextRect);
	mWindow.draw(mNextFlipRect);
	mWindow.draw(mHighscoresRect);

	mWindow.display();
}

void Game::applyBlurToTexture()
{
	const int multiplier = 2;
	mBlurStates.blendMode = sf::BlendNone;

	for (int i = 0; i < multiplier; i++)
	{
		//Horizontal blur
		mBlurShader.setUniform("offset", sf::Vector2f(2.0f / 484.f, 0.f));
		mBlurStates.shader = &mBlurShader;
		mBlurTexture.draw(mRenderSprite, mBlurStates);
		mBlurTexture.display();
		mRenderSprite.setTexture(mBlurTexture.getTexture());

		//Vertical blur
		mBlurShader.setUniform("offset", sf::Vector2f(0.f, 2.0f / 644.f));
		mBlurStates.shader = &mBlurShader;
		mBlurTexture.draw(mRenderSprite, mBlurStates);
		mBlurTexture.display();
		mRenderSprite.setTexture(mBlurTexture.getTexture());
	}

	mBlurStates.blendMode = sf::BlendAdd;
}

void Game::update()
{
	sf::Time timePerMove = sf::seconds(std::max(0.6f / ((mScore / 3000) + 1.f), 0.10f));
	positionGuide();

	if (mStatus == Status::Playing)
	{
		if (!mFadeEffects.size() && !mFallEffect)
		{
			if (mNextFlipCounter == 0)
			{
				mInverted = !mInverted;
				randomizeNextFlip();
				dropBlockSets();
				positionNewSets();
				mNextFlipCounterText.setString(std::to_string(mNextFlipCounter));
				return;
			}

			mTimeSinceLastDrop += mClock.getElapsedTime();
			mMoveTimer -= mClock.getElapsedTime();
			mRotateTimer -= mClock.getElapsedTime();
			mKeyFallTimer -= mClock.getElapsedTime();
			mClock.restart();

			while (mTimeSinceLastDrop > timePerMove)
			{
				mTimeSinceLastDrop -= timePerMove;
				mCurrentBlockSet.move(0.f, 32.f - (64.f * mInverted));
				std::vector<sf::Vector2f> positions = mCurrentBlockSet.getBlockPositions();
				bool touchdown = false;

				for (int i = 0; i < positions.size(); i++)
				{
					if (positions[i].y > 640.f || positions[i].y < 0.f)
					{
						touchdown = true;
					}
				}
				if (touchdown || blockCollision())
				{
					mCurrentBlockSet.move(0.f, -32.f + (64.f * mInverted));
					newBlockSet();
				}
			}
		}
		else if (mFallEffect)
		{
			mFallElapsedTime += mClock.getElapsedTime();
			float movement = (500.f + (200.f * mFallElapsedTime.asSeconds())) * mClock.getElapsedTime().asSeconds();
			mClock.restart();
			bool movementFound = false;

			if (mInverted)
			{
				movement *= -1.f;
			}
			for (int i = 0; i < mBlockSets.size(); i++)
			{
				mBlockSets[i].move(0.f, movement);

				if (mInverted)
				{
					mBlockSets[i].setPosition(mBlockSets[i].getPosition().x,
						std::max(mMinMaxBlockSetPositions[i].y, mBlockSets[i].getPosition().y));
				}
				else
				{
					mBlockSets[i].setPosition(mBlockSets[i].getPosition().x,
						std::min(mMinMaxBlockSetPositions[i].y, mBlockSets[i].getPosition().y));
				}
				if (mBlockSets[i].getPosition().y != mMinMaxBlockSetPositions[i].y)
				{
					movementFound = true;
				}
			}
			if (!movementFound)
			{
				for (int i = 0; i < mBlockSets.size(); i++)
				{
					mBlockSets[i].setPosition(mMinMaxBlockSetPositions[i]);
				}

				mFallEffect = false;
				updateTileSheet();
				checkFullHorizons(3);
				//If you take out 1 row, and blocks fall
				//to take out another, you get 3 times
				//as many points.

				mMinMaxBlockSetPositions.clear();
				mPreviousBlockSetPositions.clear();
				mFallElapsedTime = sf::Time::Zero;
			}
		}
		else
		{
			for (int i = 0; i < mFadeEffects.size(); i++)
			{
				mFadeEffects[i].setSize(sf::Vector2f(-320.f + (mClock.getElapsedTime().asSeconds() * 1600.f), 32.f));
			}
			if (mFadeEffects[0].getSize().x >= 0)
			{
				mClock.restart();
				mFadeEffects.clear();
				dropBlockSets();
			}
		}
	}
	else if (mStatus == Status::GameOver)
	{
		std::string s = "Game Over. Enter Name: \n        " + username;
		mDisplayText.setString(s);
	}
	else if (mStatus == Status::Waiting)
	{
		mDisplayText.setString("Press ENTER to Start.");
	}
	else if (mStatus == Status::Paused)
	{
		mDisplayText.setString("Paused. Press ESC\n    to resume.");
	}
}
