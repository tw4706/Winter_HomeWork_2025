#pragma once
#include"GameObject.h"
class Enemy:public GameObject
{
protected:
	bool isRight_;			//‰E‚ğŒü‚¢‚Ä‚¢‚é‚©‚Ç‚¤‚©

public:
	Enemy(Vector2 pos,Vector2 vel);
	virtual~Enemy();

	virtual void Init() = 0;
	virtual void Update()=0;
	virtual void Draw() = 0;

	virtual void Attack() = 0;//UŒ‚
	const Vector2& GetPos()const { return pos_; }
};

