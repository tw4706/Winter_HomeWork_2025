#include "Animation.h"
#include<Dxlib.h>

Animation::Animation(int handle,int w,int h):
	handle_(handle),
	graphW(w),
	graphH(h),
	frame_(0),
	currentFrame_(0),
	frameCount_(0),
	timer_(0),
	isTurn_(false),
	pos_({0,0})
{
}

Animation::~Animation()
{
}

void Animation::Init()
{

}

void Animation::Update()
{
	timer_++;
	if (timer_ >= frameCount_) 
	{
		timer_ = 0;
	}
	currentFrame_ = (currentFrame_ + 1) % frameCount_;
}

void Animation::Draw(Vector2 pos, bool isTurn)
{
	isTurn_ = isTurn;
	if (isTurn_)
	{
		DrawRectGraph(pos.x, pos.y,
			currentFrame_ * graphW, 0,
			graphW, graphH,
			handle_, true);
	}
	else
	{
		DrawRectGraph(pos.x, pos.y,
			currentFrame_ * graphW, 0,
			graphW, graphH,
			handle_, false);
	}
}

void Animation::Reset()
{
	currentFrame_ - 0;
	timer_ = 0;
}
