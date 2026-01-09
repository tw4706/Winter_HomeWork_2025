#include "Enemy.h"
#include"Animation.h"
#include"SpriteEffect.h"
#include"EffectManager.h"
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
	currentState_(0),
	pEffectManager_(nullptr)
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
	//敵のヒットエフェクト
	if (pEffectManager_)
	{
		pEffectManager_->AddEffect(
			std::make_shared<SpriteEffect>(
				pos_,
				"data/Effect/enemy_explosion.png",
				240, 16,
				16, 16,
				4,
				4,
				2.0f));
	}

	hp_ -= damage;

	if(hp_ <= 0)
	{
		//死ぬときのエフェクト
		if (pEffectManager_)
		{
			pEffectManager_->AddEffect(
				std::make_shared<SpriteEffect>(
					pos_,
					"data/Effect/enemy_explosion.png",
					224, 160,
					16, 16,
					4,
					4,
					2.0f));
		}
		Dead();
	}
}

void Enemy::Dead()
{
	isDead_ = true;
	colRect_.SetCenter(-9999,-9999,0,0);
}

void Enemy::SetEffectManager(EffectManager* effect)
{
	pEffectManager_ = effect;
}