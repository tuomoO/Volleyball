
#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
	GameObject();
	GameObject(sf::Texture* texture);
	~GameObject();

	sf::RectangleShape& getShape();
	void setRealPos(int x, int y);
	sf::Vector2i getRealPos();
	void move(int x, int y = 0);
	void draw(sf::RenderWindow* window);

protected:
	sf::Vector2i mRealPos;

	sf::RectangleShape mShape;
};