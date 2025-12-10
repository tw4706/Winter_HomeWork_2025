#pragma once
#include "Enemy.h"
class BulletManager;
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
	Boss(Vector2 pos, Vector2 vel,std::shared_ptr<Player>player,BulletManager*bm);
	~Boss();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Move()override {};

	void ChangeState(BossState nextState);

	void UpdateIdle();
	void UpdateAttack();
	void UpdateFly();
	void UpdateHurt();
	void UpdateDead();

	void OnHit(int damage);

private:
	int hp_;
	BossState state_;
	int stateTimer_;
	int handle_;
	float shotTimer_;
	float shotInterval_;
	Vector2 backPos_;
	std::vector<int>graphHandles_;
	std::shared_ptr<Player>pPlayer_;
	BulletManager* pBm_;
};

