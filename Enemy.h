#pragma once
#include"GameObject.h"
class Enemy:public GameObject
{
public:
	Enemy();
	virtual~Enemy();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;
	virtual void Attack()abstract;//UŒ‚
};

