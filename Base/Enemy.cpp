#include "Enemy.h"
#include"Bullet.h"
#include"Animation.h"
#include"SpriteEffect.h"
#include"EffectManager.h"
#include"GlobalConstants.h"
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
				Vector2{ pos_.x,pos_.y - 20 },
				"data/Effect/enemy_explosion.png",
				128,80,
				16, 16,
				2,
				6,
				3.0f));
	}

	hp_ -= damage;

	if(hp_ <= 0)
	{
		//死ぬときのエフェクト
		if (pEffectManager_)
		{
			pEffectManager_->AddEffect(
				std::make_shared<SpriteEffect>(
					Vector2{pos_.x,pos_.y-10},
					"data/Effect/enemy_explosion.png",
					240, 32,
					16, 16,
					4,
					5,
					4.0f));
		}
		Dead();
	}
}

void Enemy::Dead()
{
	isDead_ = true;
}

void Enemy::SetEffectManager(EffectManager* effect)
{
	pEffectManager_ = effect;
}
