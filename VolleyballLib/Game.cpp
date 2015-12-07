
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
	
	mFence.getShape().setSize(Vector2f(16, 96));
	mFence.setRealPos(Court::w / 2, Court::h);
	mFence.getShape().setOrigin(8, 96);
	mFence.getShape().setFillColor(Color(64, 64, 64));
	mLocalPlayer = 0;
	mRunning = false;
}

Game::~Game()
{

}

void Game::update(float dt)
{
	mPlayer1.update(dt);
	mPlayer2.update(dt);
}

void Game::updateState(StatePacket packet)
{
	mPlayer1.setRealPos(packet.x1, packet.y1);
	mPlayer2.setRealPos(packet.x2, packet.y2);
	mBall.setRealPos(packet.ballx, packet.bally);
}

void Game::draw(sf::RenderWindow* window)
{
	window->clear(sf::Color(0u, 127u, 255u));
	mPlayer1.draw(window);
	mPlayer2.draw(window);
	mBall.draw(window);
	mFence.draw(window);
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

void Game::start()
{
	mRunning = true;
}

void Game::stop()
{
	mRunning = false;
}

bool Game::isRunning()
{
	return mRunning;
}