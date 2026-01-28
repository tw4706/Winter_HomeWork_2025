#include "SkullFlower.h"
#include"SpriteEffect.h"
#include "SEManager.h"
#include"Application.h"
#include "BulletManager.h"
#include "EffectManager.h"
#include "Bullet.h"
#include<Dxlib.h>
#include <cassert>

namespace
{
	enum Graph
	{
		kIdleGraph,
		kHurtGraph,
		kDeathGraph,

		kGraphNum
	};

	const std::string kDogGraphName[kGraphNum] =
	{
		"data/Enemy/flower_idle.png",
		"data/Enemy/flower_hurt.png",
		"data/Enemy/flower_death.png"
	};

	const int frameCount[kGraphNum] = { 4, 5,10 };
	const int frameIntervals[kGraphNum] = { 6, 6,4 };

	// ==============================
	// 描画・当たり判定
	// ==============================
	constexpr int  kGraphSize = 64;
	constexpr int  kGraphColSize = 64 / 2;
	constexpr float kScale = 2.0f;
	constexpr float kSpeed = 5.0f;
	constexpr float kDistance = 300.0f;
	constexpr int kDrawOffsetY = 15;
	constexpr int kColOffsetY = 15;
	constexpr int kDefaultRedR = 255;
	constexpr int kDefaultRedG = 255;
	constexpr int kDefaultRedB = 255;

	// ==============================
	// 弾発射
	// ==============================
	constexpr float kShotInterval = 1.0f / 60.0f;
	constexpr float kShotCoolTime = 2.0f;

	// ==============================
	// アニメーション
	// ==============================
	constexpr int kIdleSrcY = 192;

	//========================
	// 被弾演出関連
	//========================
	constexpr int kDamageFlashInterval = 4;
	constexpr int kFlashAlpha = 128;
	constexpr int kFlashRedR = 255;
	constexpr int kFlashRedG = 64;
	constexpr int kFlashRedB = 64;
	constexpr int kFlashCycle = 2;

	// ==============================
	// 無効座標
	// ==============================
	constexpr float kDisablePos = -10000.0f;
}

SkullFlower::SkullFlower(Vector2 pos, Vector2 vel, BulletManager* bm)
	: Enemy(pos, vel),
	pBm_(bm),
	shotInterval_(kShotCoolTime),
	shotTimer_(0.0f),
	isColActive_(true),
	flowerState_(SkullFlowerState::Idle)
{
}

SkullFlower::~SkullFlower()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void SkullFlower::Init()
{
	graphHandles_.resize(kGraphNum);
	animations_.resize(static_cast<int>(SkullFlowerState::Death) + 1);

	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(kDogGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphSize,
			kGraphSize,
			frameCount[i],
			frameIntervals[i],
			kScale,
			i == kIdleGraph, kIdleSrcY);
	}

	flowerState_ = SkullFlowerState::Idle;

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y, kGraphSize, kGraphSize);
}

void SkullFlower::Update()
{
	if (isDead_) return;

	if (isColActive_)
	{
		Shot();
	}

	GameObject::Update();
	
	UpdateAnim();

	// 当たり判定の更新
	if (isColActive_)
	{
		colRect_.SetCenter(pos_.x, pos_.y - kColOffsetY, kGraphSize, kGraphSize);
	}

}

void SkullFlower::Draw()
{
	Enemy::Draw();

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y- kDrawOffsetY;

	bool isFlashRed = false;

	if (isDamageFlash_)
	{
		int t = damageFlashTimer_ / kDamageFlashInterval;
		isFlashRed = (t % kFlashCycle == 0);
	}

	if (isFlashRed)
	{
		//赤色で点滅
		SetDrawBlendMode(DX_BLENDMODE_ADD, kFlashAlpha);
		SetDrawBright(kFlashRedR, kFlashRedG, kFlashRedB);
	}

	animations_[static_cast<int>(flowerState_)]->Draw(drawX, drawY, !isTurn_);

	//点滅後元に戻す
	if (isFlashRed)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		SetDrawBright(kDefaultRedR, kDefaultRedG, kDefaultRedB);
	}

#ifdef _DEBUG
	// デバッグ用に座標表示
	DrawString(0, 360, ("X: " + std::to_string((int)drawX)).c_str(), GetColor(255, 255, 255));
	DrawString(0, 400, ("Y: " + std::to_string((int)drawY)).c_str(), GetColor(255, 255, 255));

	//当たり判定の描画
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void SkullFlower::OnHit(int damage)
{
	//死亡中は無視
	if (flowerState_ == SkullFlowerState::Death) return;

	//ヒット処理
	Enemy::OnHit(damage);

	//死亡判定後のアニメーションの処理
	if (hp_ <= 0)
	{
		colRect_.SetCenter(-kDisablePos, -kDisablePos, 0, 0);
		isColActive_ = false;

		flowerState_ = SkullFlowerState::Death;
		animations_[static_cast<int>(flowerState_)]->Reset();
	}
	else
	{
		flowerState_ = SkullFlowerState::Hurt;
		animations_[static_cast<int>(flowerState_)]->Reset();
	}
}

void SkullFlower::UpdateAnim()
{
	auto idleAnim = animations_[static_cast<int>(SkullFlowerState::Idle)];
	auto hurtAnim = animations_[static_cast<int>(SkullFlowerState::Hurt)];
	auto deathAnim = animations_[static_cast<int>(SkullFlowerState::Death)];

	switch (flowerState_)
	{
	case SkullFlowerState::Idle:
		idleAnim->Update();
		break;

	case SkullFlowerState::Hurt:
		if (!hurtAnim->IsAnimFinished())
		{
			hurtAnim->Update();
		}
		else
		{
			//Hurt状態が終わったらIdleへ戻す
			flowerState_ = SkullFlowerState::Idle;
			idleAnim->Reset();
		}
		break;

	case SkullFlowerState::Death:
		if (!deathAnim->IsAnimFinished())
		{
			deathAnim->Update();
		}
		else
		{
			//死亡アニメーション終了あとに敵を消す
			isDead_ = true;
		}
		break;
	}
}

void SkullFlower::Shot()
{
	//弾発射タイマー更新
	shotTimer_ += kShotInterval;
	float distance = std::abs(pos_.x - pPlayer_->GetPos().x);
	if (distance < kDistance)
	{
		if (shotTimer_ >= shotInterval_) 
		{
			shotTimer_ = 0.0f;

			Application::GetInstance().GetSEManager().PlaySE(SE::FlowerAttack);
			//弾を発射
			Vector2 bulletPos = pos_;
			Vector2 bulletVel = { -kSpeed, 0.0f };

			pBm_->AddEnemyBullet(bulletPos, bulletVel);
		}
	}
}
