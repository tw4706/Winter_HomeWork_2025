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

Animation::Animation(const AnimationInfo& info):
    handle_(info.handle),
    frameW_(info.frameWidth),
    frameH_(info.frameHeight),
    frameCount_(info.frameCountA),
    frameInterval_(info.frameIntervalA),
    currentFrame_(0),
    frameTimer_(0)
{
}

Animation::~Animation() {}

void Animation::Init()
{
    //初期化処理
    Reset();
}

void Animation::Update() 
{
	//タイマーを進める
    frameTimer_++;

	//タイマーがインターバルを超えたらフレームを進める
    if (frameTimer_ >= frameInterval_) 
    {
        frameTimer_ = 0;
		//フレームを進める
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
    }
}
//アニメーションを最初のフレームにリセットする
void Animation::Reset()
{
	currentFrame_ = 0;
	frameTimer_ = 0.0f;
}

