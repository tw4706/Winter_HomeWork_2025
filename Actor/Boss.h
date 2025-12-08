#pragma once
#include "Enemy.h"
class Boss :public Enemy
{
public:
	Boss(Vector2 pos, Vector2 vel);
	~Boss();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Move()override;
	void Attack();
private:

};

