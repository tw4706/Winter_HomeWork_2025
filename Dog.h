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

	void Move();//移動処理

private:
	int DogH_;//ドッグの画像ハンドル
};

