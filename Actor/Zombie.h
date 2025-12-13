#pragma once
#include "Enemy.h"
#include<memory>

enum class ZombieState
{
	Idle,
	Walk
};

class Player;
class Zombie :public Enemy


{
public:
	Zombie(Vector2 pos,Vector2 vel);
	~Zombie();

	void Init()override;
	void Update()override;
	void Draw()override;

	void UpdateAnim();

	void Move();			//ˆÚ“®ˆ—

private:
	std::vector<int>graphHandles_;
	ZombieState zombieState_;
	bool isIdlePlayed_;
};

