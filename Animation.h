#pragma once
#include"Geometry.h"

class Animation
{
public:

	Animation();
	~Animation();

    void Init(int frameW,int frameH,int frameCount,int interval);
	void Update();
    void Draw(int handle, int x, int y, float scale, float angle);

private:
    int handle_;            //画像ハンドル
    int frameW_;            //フレームの幅
    int frameH_;            //フレームの高さ
    int frameCount_;        //総フレーム数
    int currentFrame_;      //現在のフレーム
    int frameTimer_;        //フレーム更新するためのタイマー
	int frameInterval_;     //フレームの間隔
};

