
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

	void update(StatePacket state);
	void draw(sf::RenderWindow* window);

	Slime* player1();
	Slime* player2();
	Ball* ball();
	StatePacket getState();

private:
	sf::Texture mTexture;
	Slime mPlayer1;
	Slime mPlayer2;
	Ball mBall;

};