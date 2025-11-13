#include "Enemy.h"
#include<Dxlib.h>


Enemy::Enemy(Vector2 pos,Vector2 vel):
	GameObject(pos,vel),
	isTurn_(false),
	isDead_(false)
{
	pos_ = pos;
	vel_ = vel;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	if (isDead_)return;
	GameObject::Update();
}

void Enemy::Draw()
{
	if (isDead_)return;
}
