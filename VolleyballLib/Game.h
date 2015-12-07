
#pragma once

#include "Slime.h"
#include "Ball.h"
#include "Packet.h"

namespace sf
{
	class RenderWindow;
}

class Game
{
public:
	Game();
	~Game();

	void update(float dt);
	void updateState(StatePacket state);
	void draw(sf::RenderWindow* window);
	void selectLocalPlayer(Message message);
	int getLocalPlayerNumber();
	Slime* getLocalPlayer();
	Slime* player1();
	Slime* player2();
	Ball* ball();
	StatePacket getState();
	void start();
	void stop();
	bool isRunning();

private:
	sf::Texture mTexture;
	Slime mPlayer1;
	Slime mPlayer2;
	Ball mBall;
	GameObject mFence;
	int mLocalPlayer;
	bool mRunning;

};