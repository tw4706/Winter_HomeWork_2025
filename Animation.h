#pragma once
#include"Geometry.h"
class Animation
{
public:
	Animation(int handle,int w,int h);
	virtual~Animation();

	void Init();
	void Update();
	void Draw(Vector2 pos,bool isTurn);

	void Reset();

private:
	int handle_;		//画像ハンドル
	int graphW, graphH;	//画像のサイズ
	int frame_;			//
	int frameCount_;	//フレームカウンター
	int currentFrame_;	//現在のフレーム
	int timer_;			//経過時間
	bool isTurn_;		//向きの反転
	Vector2 pos_;		//座標
};

