#pragma once
#include "Enemy.h"
class Dog :public Enemy
{
public:
	Dog(Vector2 pos);
	~Dog();

	void Init()override;
	void Update()override;
	void Draw()override;

private:
	int DogH_;//ドッグの画像ハンドル
};

