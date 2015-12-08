
#pragma once

#include "GameObject.h"

class Slime : public GameObject
{
public:
	Slime();
	Slime(sf::Texture* texture, sf::Color color);
	~Slime();

	void jump();
	void update(float dt);
	bool isJumping();

private:
	bool mJumping;
	int mJumpHeight;

};