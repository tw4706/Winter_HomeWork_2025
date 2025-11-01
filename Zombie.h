#pragma once
#include "Enemy.h"
class Zombie :public Enemy
{
public:
	Zombie(Vector2 pos);
	virtual void Move()override;
};

