#pragma once
#include "Enemy.h"
class Dog :public Enemy
{
public:
	Dog(Vector2 pos,Vector2 vel);
	~Dog();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;
	void Move()override;//移動処理

private:
	int dogH_;				//ドッグの画像ハンドル
	float timer_;			//タイマー
};

