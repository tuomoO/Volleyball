
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

void GameObject::setRealPos(int x, int y)
{ 
	mRealPos = Vector2i(x, y);
}

void GameObject::move(int x, int y)
{
	mRealPos += Vector2i(x, y);
}

Vector2i GameObject::getRealPos()
{
	return mRealPos;
}

/*
Vector2f GameObject::getLocalPos()
{
	return mLocalPos;
}
*/

/*
void GameObject::move(float x)
{
	mLocalPos += Vector2f(x, 0);
}
*/

void GameObject::update()
{
	// TODO Local -> real
	mShape.setPosition(mRealPos.x, mRealPos.y);
}
