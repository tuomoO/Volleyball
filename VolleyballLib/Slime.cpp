
#include "Slime.h"
#include "Court.h"

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
	mJumping = false;
	mJumpHeight = Court::h;
}

Slime::~Slime()
{

}

void Slime::jump()
{
	if (!mJumping)
	{
		mJumping = true;
		mJumpHeight = Court::h / 2;
	}
}

void Slime::update(float dt)
{
	if (mJumping)
	{
		//ascending
		if (mJumpHeight < Court::h)
		{
			mRealPos.y -= static_cast<int>(1.0f * dt + 0.5f);
			if (mRealPos.y < mJumpHeight)
			{
				mRealPos.y = mJumpHeight;
				mJumpHeight = Court::h;
			}
		}
		//descending
		else
		{
			mRealPos.y += static_cast<int>(1.0f * dt + 0.5f);
			if (mRealPos.y > mJumpHeight)
			{
				mRealPos.y = mJumpHeight = Court::h;
				mJumping = false;
			}
		}
	}
}