#pragma once
#include "Enemy.h"
#include<memory>
class Player;
class Zombie :public Enemy
{
public:
	Zombie(Vector2 pos,Vector2 vel);
	~Zombie();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;	//UŒ‚ˆ—
	void Move();			//ˆÚ“®ˆ—

	void DrawAnimation();

private:
	int zombieH_;			//ƒ]ƒ“ƒr‚Ì‰æ‘œƒnƒ“ƒhƒ‹
	std::shared_ptr<Animation>idleAnim_;
};

