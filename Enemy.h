#pragma once
#include"GameObject.h"
class Enemy:public GameObject
{
public:
	Enemy(Vector2 pos);
	virtual~Enemy();

	virtual void Init() = 0;
	virtual void Update()=0;
	virtual void Draw() = 0;

	virtual void Attack() = 0;//UŒ‚
	virtual void Move() = 0;//ˆÚ“®
	virtual bool IsDead()const;//€–S”»’è
	const Vector2& GetPos()const { return pos_; }

protected:
	bool isRight_;	//‰E‚ğŒü‚¢‚Ä‚¢‚é‚©‚Ç‚¤‚©
};

