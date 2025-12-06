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

	void Move();			//ˆÚ“®ˆ—

private:
	int zombieH_;			//ƒ]ƒ“ƒr‚Ì‰æ‘œƒnƒ“ƒhƒ‹
	std::shared_ptr<Animation>animations_;
};

