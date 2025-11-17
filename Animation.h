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
    void Draw(Vector2 pos, float charaSize, bool isTurn, float scrollX);

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
    int frameTimer_;        //フレーム更新用タイマー
    float scale_;           //拡大率
};

