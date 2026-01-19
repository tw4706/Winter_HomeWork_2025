#pragma once
#include "Enemy.h"
#include<memory>

enum class DemoZombieState
{
	Idle,
	Walk
};

class DemoZombie :public Enemy
{
public:
	DemoZombie(Vector2 pos);
	~DemoZombie();

	void Init();
	void Update();
	void Draw();

	void Move();
	void UpdateAnim();

private:
	std::vector<int>graphHandles_;
	DemoZombieState zombieState_;
};

