#include "Animation.h"
#include<Dxlib.h>


Animation::Animation(int handle, int frameW, int frameH, int frameCount, int interval)
    : handle_(handle),
    frameW_(frameW),
    frameH_(frameH),
    frameCount_(frameCount),
    currentFrame_(0),
    frameInterval_(interval),
    frameTimer_(0),
    lastTime_(GetNowCount())
{
}

Animation::~Animation() {}

void Animation::Update() {
    int now = GetNowCount();
    frameTimer_ += now - lastTime_;
    lastTime_ = now;

    if (frameTimer_ >= frameInterval_) {
        frameTimer_ = 0;
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
    }
}


void Animation::Draw(Vector2 pos,bool isTurn)
{
    int srcX = currentFrame_ * frameW_;
    int srcY = 0;

    DrawRectGraph(
        static_cast<int>(pos.x - frameW_ / 2), // íÜêSäÓèÄÇ…ï‚ê≥
        static_cast<int>(pos.y - frameH_ / 2),
        srcX, srcY,
        frameW_, frameH_,
        handle_, TRUE,!isTurn
    );
}



void Animation::Reset()
{
	currentFrame_ = 0;
	frameTimer_ = 0.0f;
}

