#pragma once
#include"GameObject.h"
#include"Animation.h"
class Enemy:public GameObject
{
protected:
	bool isRight_;			//右を向いているかどうか
	Animation* animation_;	//アニメーションのポインタ

public:
	Enemy(Vector2 pos);
	virtual~Enemy();

	virtual void Init() = 0;
	virtual void Update()=0;
	virtual void Draw() = 0;

	virtual void Attack() = 0;//攻撃
	virtual void Move() = 0;//移動
	virtual bool IsDead()const;//死亡判定
	const Vector2& GetPos()const { return pos_; }
};

