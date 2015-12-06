
#pragma once

#include "Slime.h"
#include "Ball.h"

namespace sf
{
	class RenderWindow;
}

class Game
{
public:
	Game();
	~Game();

	void update();
	void draw(sf::RenderWindow* window);

	Slime* player1();
	Slime* player2();
	Ball* ball();

private:
	sf::Texture mTexture;
	Slime mPlayer1;
	Slime mPlayer2;
	Ball mBall;

};