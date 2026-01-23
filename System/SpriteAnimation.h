#pragma once
#include <DxLib.h>

class SpriteAnimation
{
public:
    SpriteAnimation(
        int handle,
        int frameW,
        int frameH,
        int frameCount,
        int frameInterval,
        float scale,
        int startX,
        int startY,
        bool loop);

    void Update();
    void Draw(float x, float y, bool flip = false);
    void Reset();

private:
    int handle_;
    int frameW_;
    int frameH_;
    int frameCount_;
    int frameInterval_;
    int currentFrame_;
    int frameTimer_;
    float scale_;
    int startX_;
    int startY_;
    bool isLoop_;
};

