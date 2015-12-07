
#include "Game.h"
#include "Court.h"

#include <iostream>

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
	mLocalPlayer = 0;
}

Game::~Game()
{

}

void Game::update(StatePacket packet)
{
	mPlayer1.setRealPos(packet.x1, packet.y1);
	mPlayer2.setRealPos(packet.x2, packet.y2);
	mBall.setRealPos(packet.ballx, packet.bally);
}

void Game::draw(sf::RenderWindow* window)
{
	window->clear(sf::Color(0u, 127u, 255u));
	mPlayer1.update();
	mPlayer2.update();
	mBall.update();
	window->draw(mPlayer1.getShape());
	window->draw(mPlayer2.getShape());
	window->draw(mBall.getShape());
	window->display();
}

void Game::selectLocalPlayer(Message message)
{
	if (message == Message::PLAYER1)
		mLocalPlayer = 1;
	else if (message == Message::PLAYER2)
		mLocalPlayer = 2;
	else
		cout << "Invalid value for local player!" << endl;
}

Slime* Game::getLocalPlayer()
{
	if (mLocalPlayer == 1)
		return &mPlayer1;
	else if (mLocalPlayer == 2)
		return &mPlayer2;
	else
	{
		cout << "no local player!" << endl;
		return nullptr;
	}
}

int Game::getLocalPlayerNumber()
{
	return mLocalPlayer;
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

StatePacket Game::getState()
{
	return StatePacket(mPlayer1.getRealPos().x, mPlayer1.getRealPos().y,
		mPlayer2.getRealPos().x, mPlayer2.getRealPos().y,
		mBall.getRealPos().x, mBall.getRealPos().y);
}