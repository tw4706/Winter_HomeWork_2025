#pragma once
#include"Geometry.h"
class Animation
{
public:
	Animation(int handle, int frameW, int frameH);
	~Animation();

    void InitIdle();
    void InitAttack();

	void Update();
	void Draw(Vector2 pos, bool isTurn);

	//アニメーションを最初のフレームにリセット
    void Reset();

	//現在のフレームを取得
    int GetCurrentFrame() const { return currentFrame_; }
	//総フレーム数を取得
    int GetFrameCount() const { return frameCount_; }


private:
    int handle_;            //画像ハンドル
    int frameW_;            //フレームの幅
    int frameH_;            //フレームの高さ
    int frameCount_;        //総フレーム数
    int currentFrame_;      //現在のフレーム
    int frameInterval_;     //フレーム切り替え間隔
    int frameTimer_;        //フレーム更新用タイマー
    int lastTime_;          //前回更新時刻
};

