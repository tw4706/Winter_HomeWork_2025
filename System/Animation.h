#pragma once
#include"Geometry.h"

class Animation
{
public:
    Animation(int handle, int frameW, int frameH,
        int frameCount, int frameInterval, float scale);
	~Animation();

    void Update();
    void Draw(float x, float y, bool flip);

	int GetFrameCount() const { return frameCount_; }

private:
    int handle_;            //画像ハンドル
    int frameW_;            //フレームの幅
    int frameH_;            //フレームの高さ
    int frameCount_;        //総フレーム数
    int currentFrame_;      //現在のフレーム
    int frameTimer_;        //フレーム更新するためのタイマー
	int frameInterval_;     //フレームの間隔
	float scale_;          //描画スケール
};

