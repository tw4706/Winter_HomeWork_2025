#include "SpriteEffect.h"
#include<Dxlib.h>


SpriteEffect::SpriteEffect(const Vector2& pos,const char* filePath,
    int srcX,int srcY,int frameW,int frameH,int frameCount,int frameInterval,float scale):
    srcX_(srcX),
    srcY_(srcY),
    frameW_(frameW),
    frameH_(frameH),
    frameCount_(frameCount),
    frameInterval_(frameInterval),
    scale_(scale)
{
    pos_ = pos;
    isDead_ = false;
    graphH_ = LoadGraph(filePath);
}

void SpriteEffect::Update()
{
    timer_++;

    if (timer_ >= frameInterval_)
    {
        timer_ = 0;
        frame_++;

        if (frame_ >= frameCount_)
        {
            isDead_ = true;
        }
    }
}

void SpriteEffect::Draw()
{
    if (isDead_) return;

    float drawX = pos_.x + cameraOffset_.x;
    float drawY = pos_.y + cameraOffset_.y;

    int srcX = srcX_ + frame_ * frameW_;
    int srcY = srcY_;

    DrawRectRotaGraph(
        static_cast<int>(drawX),static_cast<int>(drawY),
        srcX,srcY,
        frameW_,frameH_,
        scale_,0.0f,
        graphH_,TRUE);
}