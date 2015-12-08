
#include "Ball.h"
#include "Court.h"

#include <math.h>

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
	reset();
}

void Ball::update(float dt, Vector2i p1, Vector2i p2)
{
	if (!mHitGround)
	{
		int middle = Court::w / 2;

		//gravity
		mGravityTimer += dt;
		if (mGravityTimer > 30)
		{
			mGravityTimer = 0;
			mVelocity.y += 0.1f;
		}
		mRealPos.y += static_cast<int>(mVelocity.y * dt + 0.5f);

		//roof
		if (mRealPos.y < 16)
		{
			mRealPos.y = 16;
			mVelocity.y = 0;
		}
		//floor
		else if (mRealPos.y > Court::h - 16)
		{
			mRealPos.y = Court::h - 16;
			mHitGround = true;
			return;
		}

		//collision
		//p1 
		if (distance(p1) < 64 + 16 && mRealPos.y < p1.y)
		{
			mVelocity.y = -1.5f;
			mVelocity.x = cos(abs(mRealPos.x - p1.x) / (64 + 16));
			if (mRealPos.x < p1.x)
				mVelocity.x *= -1.0f;
		}
		//p2
		else if (distance(p2) < 64 + 16 && mRealPos.y < p2.y)
		{
			mVelocity.y = -1.5f;
			mVelocity.x = cos(abs(mRealPos.x - p2.x) / (64 + 16));
			if (mRealPos.x < p2.x)
				mVelocity.x *= -1.0f;
		}
		//fence
		else if (mRealPos.x > (middle - 8) && mRealPos.x  < (middle + 8))
		{
			if (mRealPos.y > (Court::h - 96 - 16))
			{
				mVelocity.x = 1.0f;
				if (mRealPos.x < middle)
					mVelocity.x *= -1.0f;
				mVelocity.y = -1.5f;
			}
		}

		//borders
		if (mRealPos.x < 16)
		{
			mRealPos.x = 16;
			mVelocity.x *=-1.0f;
		}
		else if (mRealPos.x > Court::w - 16)
		{
			mRealPos.x = Court::w - 16;
			mVelocity.x *= -1.0f;
		}
		mRealPos.x += static_cast<int>(0.75f * mVelocity.x * dt + 0.5f);
	}
}

float Ball::distance(Vector2i other)
{
	return sqrt(pow(mRealPos.x - other.x, 2) + pow(mRealPos.y - other.y, 2));
}

bool Ball::hasHitGround()
{
	return mHitGround;
}

void Ball::reset()
{
	mVelocity = Vector2f(0, 0);
	//mVelocityX = 0;
	//mTargetY = 0;
	mHitGround = false;
	mRealPos = Vector2i(Court::w / 2, 16);
	mGravityTimer = 0;
}