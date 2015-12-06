
#include "Game.h"
#include "Court.h"

#include <SFML\Graphics.hpp>

using namespace sf;
using namespace std;

Game::Game()
{
	mTexture.loadFromFile("circle.png");
	mPlayer1 = Slime(&mTexture, Color(0, 255, 0));
	mPlayer1.setRealPos(64, Court::h);
	mBall = Ball(&mTexture);
	mBall.setRealPos(Court::w / 2, Court::h / 2);
	mPlayer2 = Slime(&mTexture, Color(255, 0, 0));
	mPlayer2.setRealPos(Court::w - 64, Court::h);
}

Game::~Game()
{

}

void Game::update()
{
	mPlayer1.update();
	mPlayer2.update();
	mBall.update();
}

void Game::draw(sf::RenderWindow* window)
{
	window->draw(mPlayer1.getShape());
	window->draw(mPlayer2.getShape());
	window->draw(mBall.getShape());
}

Slime* Game::player1()
{
	return &mPlayer1;
}

Slime* Game::player2()
{
	return &mPlayer2;
}

Ball* Game::ball()
{
	return &mBall;
}
