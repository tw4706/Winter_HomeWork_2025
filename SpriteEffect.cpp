#include "SpriteEffect.h"
#include<Dxlib.h>

SpriteEffect::SpriteEffect(const Vector2& pos, const char* filePath, 
	int startY, int frameW, int frameH, int frameCount, int frameInterval, float scale)
{
	pos_ = pos;
	isDead_ = false;
	graphH_ = LoadGraph(filePath);
    animation_ = std::make_unique<Animation>(
        graphH_,
        frameW,
        frameH,
        frameCount,
        frameInterval,
        scale,
        false,      //ループしない
        startY);
}

void SpriteEffect::Update()
{
    animation_->Update();
    //アニメーションが終了したらエフェクトを消す
    if (animation_->IsAnimFinished())
    {
        isDead_ = true;
	}
}

void SpriteEffect::Draw()
{
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	animation_->Draw(drawX, drawY, false);
}
