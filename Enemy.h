#pragma once
#include"Character.h"
class Enemy:public Character
{
public:
	Enemy();
	virtual~Enemy();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;
	virtual void Attack()abstract;//UŒ‚
};

