#pragma once
#include "Enemy.h"
#include<memory>
class Player;
class Zombie :public Enemy
{
public:
	Zombie(Vector2 pos,Vector2 vel);
	virtual ~Zombie();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;	//UŒ‚ˆ—
	void Move();			//ˆÚ“®ˆ—

	//“–‚½‚è”»’èæ“¾
	const Rect& GetColRect() const override { return colRect_; }

	//’e‚ª“–‚½‚Á‚½‚Ìˆ—
	void IsHit()override;

	//€–S”»’èæ“¾
	bool IsDead()const override { return isDead_; }

private:
	int zombieH_;			//ƒ]ƒ“ƒr‚Ì‰æ‘œƒnƒ“ƒhƒ‹
};

