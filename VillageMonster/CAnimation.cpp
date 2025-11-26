#include "Animation.h"
#include<Dxlib.h>

Animation::Animation(int handle, int frameW,
	int frameH, int frameCount, int interal) :
	handle_(handle),
	frameW_(frameW),
	frameH_(frameH),
	frameCount_(frameCount),
	frameInterval_(interal),
	currentFrame_(0),
	frameTimer_(0.0f)
{
}

Animation::~Animation()
{
}

void Animation::Update()
{
	frameTimer_++;
	if (frameTimer_ >= frameInterval_)
	{
		frameTimer_ = 0.0f;
		currentFrame_ = (currentFrame_ + 1) % frameCount_;
	}
}

void Animation::Draw(float scaleX, float scaleY)
{
	int srcX = currentFrame_ * frameW_;
	int srcY = 0;
	DrawRectRotaGraph3(0, 0, srcX, srcY, frameW_, frameH_,
		1.0f, 0.0f, scaleX, scaleY, handle_, TRUE, FALSE);
}

void Animation::Reset()
{
	currentFrame_ = 0;
	frameTimer_ = 0.0f;
}
