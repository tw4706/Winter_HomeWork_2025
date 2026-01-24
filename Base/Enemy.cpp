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
	// ==============================
	// ステータス関連
	// ==============================
	constexpr int kMaxHp = 3;

	// ==============================
	// Effekseer関連
	// ==============================
	constexpr char kHitEffectPath[] = "data/Effect/EnemyHit.efk";
	constexpr float kHitEffectScale = 0.5f;

	// ==============================
	// 死亡演出関連
	// ==============================
	constexpr float kDeadEffectOffsetY = 10.0f;

	// ==============================
	// 爆発エフェクト関連
	// ==============================
	constexpr char kExplosionImagePath[] = "data/Effect/enemy_explosion.png";
	constexpr int kExplosionImageW = 240;
	constexpr int kExplosionImageH = 32;
	constexpr int kExplosionFrameW = 16;
	constexpr int kExplosionFrameH = 16;
	constexpr int kExplosionFrameCount = 4;
	constexpr int kExplosionframeInterval = 5;
	constexpr float kExplosionScale = 4.0f;

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
	effect3DHandle_ = LoadEffekseerEffect(kHitEffectPath);
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
	SetScalePlayingEffekseer2DEffect(playing3DHandle_,
		kHitEffectScale, kHitEffectScale, 0.0f);

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
				Vector2{ pos_.x,pos_.y - kDeadEffectOffsetY },
				kExplosionImagePath,
				kExplosionImageW,kExplosionImageH,
				kExplosionFrameW,kExplosionFrameH,
				kExplosionFrameCount,
				kExplosionframeInterval,
				kExplosionScale));
	}
}

void Enemy::SetEffectManager(EffectManager* effect)
{
	pEffectManager_ = effect;
}
