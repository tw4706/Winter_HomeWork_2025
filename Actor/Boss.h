#pragma once
#include "Enemy.h"
class Boss :public Enemy
{
public:
	enum class BossState
	{
		Idle,
		Walk,
		Fly,
		Attack,
		Spawn,
		Hurt,
		Dead
	};
	Boss(Vector2 pos, Vector2 vel);
	~Boss();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Move()override;
	void Attack();

	void UpdateState();
	void ChangeState();

private:
	BossState state_;
	int stateTimer_;
};

