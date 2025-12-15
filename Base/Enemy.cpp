#include "Enemy.h"
#include"Animation.h"
#include<Dxlib.h>

namespace
{
	constexpr int kMaxHp = 3;
}

Enemy::Enemy(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel),
	isTurn_(false),
	isDead_(false),
	hp_(kMaxHp),
	currentState_(0)
{

}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	if (isDead_)return;

	Move();

	GameObject::Update();

	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);
}

void Enemy::Draw()
{
	if (isDead_)return;
}

void Enemy::OnHit(int damage)
{
	hp_ -= damage;

	if(hp_ <= 0)
	{
		Dead();
	}
}

void Enemy::Dead()
{
	isDead_ = true;
	colRect_.SetCenter(-9999,-9999,0,0);
}