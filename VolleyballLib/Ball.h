
#pragma once

#include "GameObject.h"

class Ball : public GameObject
{
public:
	Ball();
	Ball(sf::Texture* texture);
	~Ball() = default;
	void update(float dt, sf::Vector2i p1, sf::Vector2i p2);
	bool hasHitGround();
	void reset();

private:
	float distance(sf::Vector2i other);

	sf::Vector2f mVelocity;
	bool mHitGround;
	float mGravityTimer;
};