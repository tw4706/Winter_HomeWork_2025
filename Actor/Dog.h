#pragma once
#include "Enemy.h"

enum class DogState
{
	Idle,
	Jump
};

class Dog :public Enemy
{
public:
	Dog(Vector2 pos,Vector2 vel);
	~Dog();

	void Init()override;
	void Update()override;
	void Draw()override;

	//移動処理
	void Move()override;

	void UpdateAnim();
private:
	std::vector<int>graphHandles_;
	DogState dogState_;
	float timer_;//タイマー
};

