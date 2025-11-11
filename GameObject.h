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

protected:
	virtual void Gravity();//重力
};

