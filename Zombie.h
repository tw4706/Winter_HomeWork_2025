#pragma once
#include "Enemy.h"
class Zombie :public Enemy
{
public:
	Zombie(Vector2 pos);
	virtual ~Zombie();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;
	void Move()override;

private:
	int zombieH_;	//ƒ]ƒ“ƒr‚Ì‰æ‘œƒnƒ“ƒhƒ‹

};

