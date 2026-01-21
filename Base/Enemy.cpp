#include "Enemy.h"
#include"Bullet.h"
#include"Animation.h"
#include"Application.h"
#include"SpriteEffect.h"
#include"EffectManager.h"
#include"GlobalConstants.h"
#include<Dxlib.h>
#include "EffekseerForDXLib.h"

namespace
{
	constexpr int kMaxHp = 3;
}

Enemy::Enemy(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel),
	effect3DHandle_(-1),
	playing3DHandle_(-1),
	isTurn_(false),
	isDead_(false),
	hp_(kMaxHp),
	currentState_(0),
	pEffectManager_(nullptr)
{
	effect3DHandle_ = LoadEffekseerEffect("data/Effect/EnemyHit.efk");
}

Enemy::~Enemy()
{
	DeleteEffekseerEffect(effect3DHandle_);
}

void Enemy::Update()
{
	UpdateEffekseer2D();

	if (isDead_)return;

	GameObject::Update();

	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);
}

void Enemy::Draw()
{
	if (isDead_)return;

	DrawEffekseer2D();
}

void Enemy::OnHit(int damage)
{
	if (isDead_)return;

	Application::GetInstance().GetSEManager().PlaySE(SE::Hit);

	//敵のヒットエフェクト
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	playing3DHandle_ = PlayEffekseer2DEffect(effect3DHandle_);
	SetPosPlayingEffekseer2DEffect(playing3DHandle_, drawX, drawY, 0.0f);
	SetScalePlayingEffekseer2DEffect(playing3DHandle_, 0.5f, 0.5f, 0.0f);

	hp_ -= damage;

	if(hp_ <= 0)
	{
		Dead();
	}
}

void Enemy::Dead()
{
	isDead_ = true;
	//死ぬときのエフェクト
	if (pEffectManager_)
	{
		pEffectManager_->AddEffect(
			std::make_shared<SpriteEffect>(
				Vector2{ pos_.x,pos_.y - 10 },
				"data/Effect/enemy_explosion.png",
				240, 32,
				16, 16,
				4,
				5,
				4.0f));
	}
}

void Enemy::SetEffectManager(EffectManager* effect)
{
	pEffectManager_ = effect;
}
