#include "SpriteAnimation.h"

SpriteAnimation::SpriteAnimation(
    int handle,
    int frameW,int frameH,
    int frameCount,int frameInterval,
    float scale,
    int startX,int startY,
    bool loop):
    handle_(handle),
    frameW_(frameW),
    frameH_(frameH),
    frameCount_(frameCount),
    frameInterval_(frameInterval),
    currentFrame_(0),
    frameTimer_(0),
    scale_(scale),
    startX_(startX),
    startY_(startY),
    isLoop_(loop)
{
}

void SpriteAnimation::Update()
{
    frameTimer_++;
    if (frameTimer_ < frameInterval_)
    {
        return;
    }

    frameTimer_ = 0;

    if (!isLoop_ && currentFrame_ >= frameCount_ - 1)
    {
        return;
    }

    currentFrame_++;

    if (isLoop_ && currentFrame_ >= frameCount_)
    {
        currentFrame_ = 0;
    }
}

void SpriteAnimation::Draw(float x, float y, bool flip)
{
    int sx = startX_ + currentFrame_ * frameW_;
    int sy = startY_;

    DrawRectRotaGraph3(
        static_cast<int>(x),
        static_cast<int>(y),
        sx, sy,
        frameW_, frameH_,
        frameW_ / 2, frameH_ / 2,
        scale_, scale_,
        0.0,
        handle_,
        TRUE,
        flip);
}

void SpriteAnimation::Reset()
{
    currentFrame_ = 0;
    frameTimer_ = 0;
}