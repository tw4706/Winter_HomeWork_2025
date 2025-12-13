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

	// アニメーション更新
	if (!animations_.empty() && currentState_ >= 0 && currentState_ < (int)animations_.size())
		animations_[currentState_]->Update();
}

void Enemy::Draw()
{
	if (isDead_)return;

	if (!animations_.empty() && currentState_ >= 0 && currentState_ < (int)animations_.size())
	{
		animations_[currentState_]->Draw(pos_.x, pos_.y, !isTurn_);
	}
}

void Enemy::OnHit(int damage)
{
	hp_ -= damage;
	if (hp_ <= 0)
	{
		isDead_ = true;
		colRect_.SetCenter(-9999, -9999, 0, 0);
	}

}