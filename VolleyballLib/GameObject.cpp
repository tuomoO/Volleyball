
#include "GameObject.h"
#include "Court.h"

using namespace sf;

GameObject::GameObject()
{
	mRealPos = Vector2i(0, 0);
}

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

void GameObject::draw(RenderWindow* window)
{
	mShape.setPosition(mRealPos.x, mRealPos.y);
	window->draw(mShape);
}
