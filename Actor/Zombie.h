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

private:
	std::vector<int>graphHandles_;
	ZombieState zombieState_;
	bool isInvincibled_; //無敵状態かどうか
	bool isIdleAnimPlayed_; //Idleアニメーションが再生されたかどうか
	int idleReverseFrame_;   // 逆再生中の現在フレーム
	int idleReverseTimer_;  // 逆再生用タイマー
	bool isReverseIdle_;    // Idleを逆再生中か
};

