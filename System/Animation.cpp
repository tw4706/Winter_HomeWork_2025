#include "Animation.h"
#include<Dxlib.h>


Animation::Animation(int handle, int frameW, int frameH, 
    int frameCount, int frameInterval,float scale,bool loop,int startY) :
    handle_(handle), frameW_(frameW), frameH_(frameH),
    frameCount_(frameCount), frameInterval_(frameInterval),
	currentFrame_(0), frameTimer_(0), scale_(scale), isLoop_(loop), startY_(startY)
{
}

Animation::~Animation() {}

void Animation::Update()
{
    frameTimer_++;

	//フレーム更新間隔に達していなければ何もしない
    if (frameTimer_ < frameInterval_)
    {
        return;
	}

	frameTimer_ = 0;

	//ループしない場合、最後のフレームで止まるようにする
    if (!isLoop_ && currentFrame_ >= frameCount_ - 1)
    {
        return;
    }

    currentFrame_++;

	//ループする場合、最後のフレームを超えたら最初のフレームに戻る
    if (isLoop_ && currentFrame_ >= frameCount_)
    {
        currentFrame_ = 0;
    }
}

void Animation::Draw(float x, float y, bool flip)
{
    int sx = currentFrame_ * frameW_;
    int sy = startY_;

    DrawRectRotaGraph3(
        (int)x, (int)y,             //描画位置
        sx, sy,                     //切り取り開始座標
        frameW_, frameH_,           //切り取りサイズ
        frameW_ / 2, frameH_ / 2,   //回転中心
        scale_, scale_,             //拡大縮小
        0.0,                        //角度
        handle_,                    //画像ハンドル
        TRUE,flip);
}

void Animation::Reset()
{
    currentFrame_ = 0;
	frameTimer_ = 0;
}

void Animation::SetFrame(int frame)
{
    if (frame < 0) 
    {
        frame = 0;
    }

    if (frame >= frameCount_)
    {
        frame = frameCount_ - 1;
    }

    currentFrame_ = frame;
    frameTimer_ = 0; //更新タイマーもリセットする
}

