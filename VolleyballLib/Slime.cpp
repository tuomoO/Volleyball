
#include "Slime.h"

using namespace sf;

Slime::Slime()
{

}

Slime::Slime(Texture* texture, sf::Color color) : GameObject(texture)
{
	mShape.setSize(Vector2f(128, 64));
	mShape.setTextureRect(Rect<int>(0, 0, 64, 32));
	mShape.setOrigin(64, 64);
	mShape.setFillColor(color);
}

Slime::~Slime()
{

}
