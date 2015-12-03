
#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
	GameObject(sf::Texture* texture);
	~GameObject();

	sf::RectangleShape& getShape();
	void setPosition(float x, float y);
	void move(float x);

protected:
	sf::RectangleShape mShape;
};