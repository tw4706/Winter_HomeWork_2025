#include "Enemy.h"
#include"Animation.h"
#include<Dxlib.h>

Enemy::Enemy(Vector2 pos,Vector2 vel):
	GameObject(pos,vel),
	isTurn_(false),
	isDead_(false),
	idleH_(-1),
	jumpH_(-1),
	attackH_(-1)
{

}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	GameObject::Update();
	if (isDead_)return;
}

void Enemy::Draw()
{
	if (isDead_)return;
}