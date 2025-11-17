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

Animation::Animation(int handle, int frameW, int frameH):
    handle_(handle),
    frameW_(frameW),
    frameH_(frameH),
    frameCount_(0),
    currentFrame_(0),
    frameTimer_(0),
	frameInterval_(0)
{
}

Animation::~Animation() {}

void Animation::Init(const AnimationSetting&set)
{
    //初期化処理
	frameCount_ = set.frameCount;
	frameInterval_ = set.frameInterval;
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

void Animation::Draw(Vector2 pos,float charaSize,bool isTurn,float scrollX)
{
    if (handle_ < 0) return;
    float drawX = pos.x - scrollX - charaSize * 0.5f;

	//描画位置と描画するフレームを計算
    int srcX = currentFrame_ * frameW_;
    int srcY = 0;

	//画像の描画
    DrawRectGraph(static_cast<int>(drawX), //画像の中心
        static_cast<int>(pos.y - frameH_ / 2),
        srcX, srcY,
        frameW_, frameH_,
        handle_, TRUE,!isTurn
    );
}

//アニメーションを最初のフレームにリセットする
void Animation::Reset()
{
	currentFrame_ = 0;
	frameTimer_ = 0.0f;
}

