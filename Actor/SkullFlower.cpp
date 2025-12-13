#include "SkullFlower.h"
#include "BulletManager.h"
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


	const std::string kGraphName[kGraphNum] =
	{
		"data/Enemy/flower_idle.png",
		"data/Enemy/flower_hurt.png",
		"data/Enemy/flower_death.png"
	};

	const int frameCount[kGraphNum] = { 4, 5,10 };
	const int frameIntervals[kGraphNum] = { 6, 6,4 };


	constexpr int  kGraphSize = 64;
	constexpr int  kGraphColSize = 64 / 2;
	constexpr float kScale = 2.0f;
	constexpr int kPosYOffset = 15;
	constexpr float kSpeed = 5.0f;
	constexpr float kDistance = 300.0f;
}

SkullFlower::SkullFlower(Vector2 pos, Vector2 vel, BulletManager* bm)
	: Enemy(pos, vel),
	pBm_(bm),
	shotInterval_(2.0f),
	shotTimer_(0.0f),
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
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphSize,
			kGraphSize,
			frameCount[i],
			frameIntervals[i],
			kScale,
			i == kIdleGraph
		);
	}

	flowerState_ = SkullFlowerState::Idle;
	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y, kGraphSize, kGraphSize);
}

void SkullFlower::Update()
{
	if (isDead_) return;

	//弾発射タイマー更新
	shotTimer_ += 1.0f / 60.0f;
	float distance = std::abs(pos_.x - pPlayer_->GetPos().x);
	if (distance < kDistance)
	{
		if (shotTimer_ >= shotInterval_) {
			shotTimer_ = 0.0f;

			//弾を発射
			Vector2 bulletPos = pos_;
			Vector2 bulletVel = { -kSpeed, 0.0f };

			pBm_->AddEnemyBullet(bulletPos, bulletVel);
		}
	}


	flowerState_ = (flowerState_ != SkullFlowerState::Death && 
		flowerState_ != SkullFlowerState::Hurt)? SkullFlowerState::Idle : flowerState_;

	shotTimer_ -= 1.0f / 60.0f;

	Enemy::Update();// 移動＋衝突更新

	// 当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y - kPosYOffset, kGraphSize, kGraphSize);

}

void SkullFlower::Draw()
{
	if (isDead_) return;
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	animations_[static_cast<int>(flowerState_)]->Draw(drawX, drawY, !isTurn_);

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
	hp_ -= damage;

	if (hp_ <= 0)
	{
		flowerState_ = SkullFlowerState::Death;
		animations_[static_cast<int>(flowerState_)]->Reset();
		isDead_ = true;
	}
	else
	{
		flowerState_ = SkullFlowerState::Hurt;
		animations_[static_cast<int>(flowerState_)]->Reset();
	}
}
