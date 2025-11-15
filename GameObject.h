#pragma once
#include"Geometry.h"
#include"Rect.h"
class GameObject
{
protected:
	Vector2 pos_;
	Vector2 vel_;
	bool isTurn_;//反転してるかどうか
	bool isGround_;//地面に接地しているかどうか
	Rect colRect_;//当たり判定の矩形

public:
	GameObject(Vector2 pos,Vector2 vel);
	virtual~GameObject();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	//当たり判定の矩形を取得する関数
	const Rect& GetColRect() const { return colRect_; }

	//位置を取得する関数(ゲッター関数)
	const Vector2& GetPos()const { return pos_; }

protected:
	virtual void Gravity();//重力
};

