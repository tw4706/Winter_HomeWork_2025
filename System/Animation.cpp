#include "Animation.h"
#include<Dxlib.h>

namespace
{
	//待機アニメーションのフレーム数・切り替え間隔
    constexpr int kIdleCount = 7;
    constexpr int kIdleInterval = 7;

	//攻撃アニメーションのフレーム数点切り替え間隔
	constexpr int kAttackCount = 8;
	constexpr int kAttackInterval = 8;
}

Animation::Animation():
    handle_(-1),
    frameW_(0),
    frameH_(0),
    frameCount_(0),
    frameInterval_(0),
    currentFrame_(0),
    frameTimer_(0)
{
}

Animation::~Animation() {}

void Animation::Init(int frameW, int frameH, int frameCount, int interval)
{
    frameW_ = frameW;
    frameH_ = frameH;
    frameCount_ = frameCount;
    frameInterval_ = interval;
    currentFrame_ = 0;
    frameTimer_ = 0;
}

void Animation::Update() 
{
	//タイマーを進める
    frameTimer_++;

	//タイマーがインターバルを超えたらフレームを進める
    if (frameTimer_ >= frameInterval_) 
    {
		//フレームを進める
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
        frameTimer_ = 0;
    }
}

void Animation::Draw(int handle,int x,int y,float scale,float angle)
{
    int srcX = currentFrame_ * frameW_;
    int srcY = 0;

    //描画
    DrawRectRotaGraph3(x, y,frameW_ / 2, frameH_ / 2,srcX,srcY,
        frameW_,frameH_,scale, angle,handle, TRUE,FALSE);
}

