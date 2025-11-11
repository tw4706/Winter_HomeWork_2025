#pragma once
#include "Enemy.h"
class Skull :public Enemy
{
public:
	Skull(Vector2 pos,Vector2 vel);
	~Skull();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;
	void Move()override;

	//’e‚ª“–‚½‚Á‚½‚Ìˆ—
	virtual void IsHit() = 0;
};

