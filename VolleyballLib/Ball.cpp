
#include "Ball.h"

using namespace sf;

Ball::Ball()
{
	mRealPos = Vector2i(0, 0);
}

Ball::Ball(Texture* texture) : GameObject(texture)
{
	mShape.setSize(Vector2f(32, 32));
	mShape.setOrigin(16, 16);
	mShape.setFillColor(Color(255, 255, 0));
}

Ball::~Ball()
{

}