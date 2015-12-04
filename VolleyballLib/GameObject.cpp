
#include "GameObject.h"
#include "Court.h"

using namespace sf;

GameObject::GameObject(Texture* texture)
{
	mShape.setTexture(texture);
}

GameObject::~GameObject()
{

}

RectangleShape& GameObject::getShape()
{
	return mShape;
}

void GameObject::setPosition(float x, float y)
{ 
	mShape.setPosition(x, y);
}

void GameObject::move(float x)
{
	mShape.move(x, 0);
	if (mShape.getPosition().x < 64)
		mShape.setPosition(64, mShape.getPosition().y);
	else if (mShape.getPosition().x > Court::w - 64)
		mShape.setPosition(Court::w - 64, mShape.getPosition().y);
}

float GameObject::getX()
{
	return mShape.getPosition().x;
}

float GameObject::getY()
{
	return mShape.getPosition().y;
}