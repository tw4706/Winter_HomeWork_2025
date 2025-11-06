#include "Enemy.h"
#include<Dxlib.h>

Enemy::Enemy(Vector2 pos,Vector2 vel):
	GameObject(pos,vel),
	isRight_(false),
	pPlayer_(nullptr)
{
	pos_ = pos;
	vel_ = vel;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	GameObject::Update();
}

void Enemy::Draw()
{
}

void Enemy::SetPlayer(Player* pPlayer)
{
	pPlayer_ = pPlayer;
}
