#pragma once
#include"Geometry.h"

struct AnimationInfo
{
    int handle;     //画像ハンドル
    int frameWidth;     //フレームの幅
    int frameHeight;     //フレームの高さ
    int frameCountA;//総フレーム数
    int frameIntervalA;//フレームの間隔
    int currentFrameA;
};;

class Animation
{
public:

	Animation(const AnimationInfo& info);
	~Animation();

    void Init();

	void Update();

	//アニメーションを最初のフレームにリセット
    void Reset();

	//現在のフレームを取得
    int GetCurrentFrame() const { return currentFrame_; }
	//総フレーム数を取得
    int GetFrameCount() const { return frameCount_; }


    AnimationInfo GetInfo() const { return { handle_, frameW_, frameH_, frameCount_, frameInterval_, currentFrame_ };}



private:
    int handle_;            //画像ハンドル
    int frameW_;            //フレームの幅
    int frameH_;            //フレームの高さ
    int frameCount_;        //総フレーム数
    int currentFrame_;      //現在のフレーム
    int frameTimer_;        //フレーム更新するためのタイマー
	int frameInterval_;     //フレームの間隔
};

