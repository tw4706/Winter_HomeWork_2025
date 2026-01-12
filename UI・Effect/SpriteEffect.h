#pragma once
#include "Effect.h"
#include "Animation.h"
#include <memory>

class SpriteEffect :public Effect
{
public:
    SpriteEffect(
        const Vector2& pos,
        const char* filePath,
        int srcX,
        int srcY,
        int frameW,
        int frameH,
        int frameCount,
        int frameInterval,
        float scale = 1.0f);

    void Update() override;
    void Draw() override;

    Vector2 GetPos() const { return pos_; }
    int GetFrameWidth() const { return frameW_; }
    int GetFrameHeight() const { return frameH_; }
    float GetScale() const { return scale_; }

private:
	//アニメーションに必要な変数
    int graphH_;

    int srcX_;
    int srcY_;
    int frameW_;
    int frameH_;
    int frameCount_;
    int frameInterval_;
    float scale_;

    int frame_ = 0;
    int timer_ = 0;
};

