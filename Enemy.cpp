#include "Enemy.h"
#include<Dxlib.h>

Enemy::Enemy(Vector2 pos):
	GameObject(pos,Vector2()),
	isRight_(false)
{
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Move();
}

void Enemy::Move()
{

}

bool Enemy::IsDead() const
{
	return false;
}
