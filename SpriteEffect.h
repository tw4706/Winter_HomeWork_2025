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
        int startY,
        int frameW,
        int frameH,
        int frameCount,
        int frameInterval,
        float scale = 1.0f
    );

    void Update() override;
    void Draw() override;

private:
    int graphH_;
    std::unique_ptr<Animation> animation_;
};

