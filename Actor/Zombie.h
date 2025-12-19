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

	void Move();			//移動処理
	void OnHit(int damage)override;	//ダメージ処理

private:
	std::vector<int>graphHandles_;
	ZombieState zombieState_;
	bool isIdlePlayed_;//Idアニメーション再生済みかどうか
	bool isInvincibled_; //無敵状態かどうか
};

