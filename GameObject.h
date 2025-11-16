#pragma once
#include"Geometry.h"
#include"Rect.h"
#include"Bg.h"
#include<memory>
#include<vector>

class GameObject
{
protected:
	Vector2 pos_;
	Vector2 vel_;
	Rect colRect_;//当たり判定の矩形
	bool isTurn_;//反転してるかどうか
	bool isGround_;//地面に接地しているかどうか
	float scrollX_;//スクロール量(横スクロールゲームなので一旦横だけ)

public:
	GameObject(Vector2 pos,Vector2 vel);
	virtual~GameObject();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	//重力
	virtual void Gravity();

	//当たり判定の矩形を取得する関数
	const Rect& GetColRect() const { return colRect_; }

	//位置を取得する関数(ゲッター関数)
	const Vector2& GetPos()const { return pos_; }

	//Bgクラスを設定する関数
	void SetBg(std::shared_ptr<Bg>bg) { pBg_ = bg; }
	//Bgクラスを取得する関数
	std::shared_ptr<Bg>GetBg() const { return pBg_; }



protected:
	std::shared_ptr<Bg>pBg_;//背景オブジェクトのポインタ
};

