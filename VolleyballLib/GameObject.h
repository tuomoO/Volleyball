
#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
	GameObject(sf::Texture* texture);
	~GameObject();

	sf::RectangleShape& getShape();
	void setRealPos(int x, int y);
	//void setVisualPos(float x, float y);
	sf::Vector2i getRealPos();
	//sf::Vector2f getLocalPos();
	void move(int x, int y);
	void update();

protected:
	sf::Vector2i mRealPos;
	//sf::Vector2f mLocalPos;

	sf::RectangleShape mShape;
};