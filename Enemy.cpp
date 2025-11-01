#include "Enemy.h"

namespace
{
	//エネミーの開始位置
	constexpr int kStartX = 500;
	constexpr int kStartY = 300;
}

Enemy::Enemy(Vector2 pos):
	GameObject(pos,Vector2())
{
}

Enemy::~Enemy()
{
}

bool Enemy::IsDead() const
{
	return false;
}
