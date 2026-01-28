#include "Dog.h"
#include "Player.h"
#include "Enemy.h"
#include"Application.h"
#include<Dxlib.h>
#include<cmath>
#include<cassert>

namespace
{
	enum Graph
	{
		kIdleGraph,
		kJumpGraph,

		kGraphNum
	};

	const std::string kDogGraphName[kGraphNum] =
	{
		"data/Enemy/dog_correction.png",
		"data/Enemy/dog_jump_correction.png"
	};
	const std::string kWeakDogGraphName[kGraphNum] =
	{
		"data/Enemy/dog.png",
		"data/Enemy/dog_jump.png"
	};

	//====================
	// アニメーション関連
	//====================
	const int frameCounts[kGraphNum] = { 11, 6 };
	const int frameIntervals[kGraphNum] = { 8, 6 };

	//====================
	// 当たり判定・描画関連
	//====================
	constexpr int kGraphW = 64;
	constexpr int kGraphH = 48;
	constexpr int kGraphColSize = 48;
	constexpr float kScale = 2.0f;
	constexpr float kDrawW = kGraphW * 2.0f;
	constexpr float kDrawH = kGraphH * 2.0f;
	constexpr float kColOffsetY = 5.0f;				//当たり判定
	constexpr float kDrawOffsetY = 30.0f;			//描画位置
	constexpr int kDefaultRedR = 255;
	constexpr int kDefaultRedG = 255;
	constexpr int kDefaultRedB = 255;

	//====================
	// ステータス関連
	//====================
	constexpr float kJumpPower = 15.0f;				//ジャンプの高さ
	constexpr float kSpeed = 4.0f;					//移動速度
	constexpr int kJumpInterval = 60;				//ジャンプ間隔
	const float kDistance = 300.0f;					//プレイヤーとの距離

	//========================
	// 被弾演出関連
	//========================
	constexpr int kDamageFlashInterval = 4;
	constexpr int kFlashAlpha = 128;
	constexpr int kFlashRedR = 255;
	constexpr int kFlashRedG = 64;
	constexpr int kFlashRedB = 64;
	constexpr int kFlashCycle = 2;
}

Dog::Dog(Vector2 pos, Vector2 vel,DogType type) :
	Enemy(pos, vel),
	dogType_(type),
	dogState_(DogState::Idle),
	timer_(0.0f)
{
	if (dogType_ == DogType::Weak)
	{
		hp_ = 1;
	}
}

Dog::~Dog()
{
	for (auto& handle : dogGraphHandles_)
	{
		DeleteGraph(handle);
	}
}

void Dog::Init()
{
	dogGraphHandles_.resize(kGraphNum);
	animations_.resize(static_cast<int>(DogState::Jump) + 1);

	const std::string* graphNames =(dogType_ == DogType::Weak) ? kWeakDogGraphName : kDogGraphName;

	for (int i = 0; i < kGraphNum; i++)
	{
		dogGraphHandles_[i] = LoadGraph(graphNames[i].c_str());

		animations_[i] = std::make_shared<Animation>(
			dogGraphHandles_[i],
			kGraphW,
			kGraphH,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			i == kIdleGraph, 0);
	}
	dogState_ = DogState::Idle;
}

void Dog::Update()
{
	if (isDead_)return;

	//移動処理
	Move();

	Enemy::Update();

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y- kColOffsetY, kGraphColSize, kGraphColSize);

	UpdateAnim();

#ifdef _DEBUG
	//デバッグ表示
	DrawFormatString(0, 150, 0xffffff, "Dog PosX:%f", pos_.x);
	DrawFormatString(0, 170, 0xffffff, "Dog VelX:%f", vel_.x);
	DrawFormatString(0, 190, 0xffffff, "Dog Dist:%f", std::abs(pPlayer_->GetPos().x - pos_.x));
#endif
}

void Dog::Draw()
{
	Enemy::Draw();

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = (pos_.y + cameraOffset_.y)- kDrawOffsetY;

	switch (dogType_)
	{
	case DogType::Normal:
		break;
	case DogType::Weak:
		break;
	default:
		break;
	}

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

	animations_[static_cast<int>(dogState_)]->Draw(drawX, drawY, isTurn_);

	//点滅後元に戻す
	if (isFlashRed)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		SetDrawBright(kDefaultRedR, kDefaultRedG, kDefaultRedB);
	}

#ifdef _DEBUG
	//当たり判定の描画
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Dog::Move()
{
	if (!pPlayer_) return;

	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = std::abs(dx);

	if (distance < kDistance)
	{
		if (isGround_)
		{
			timer_++;
			if (timer_ > kJumpInterval)
			{
				vel_.y = -kJumpPower;
				vel_.x = (dx > 0) ? kSpeed : -kSpeed;
				isTurn_ = (dx > 0) ? true : false;
				isGround_ = false;
				timer_ = 0;
				dogState_ = DogState::Jump;
				animations_[static_cast<int>(DogState::Jump)]->Reset();

				Application::GetInstance().GetSEManager().PlaySE(SE::EnemyJump);
			}
			else
			{
				//ジャンプまでの待機時間は横移動しない
				vel_.x = 0.0f;
			}
		}

	}
	else
	{
		vel_.x = 0.0f;

		//着地したらIdleに戻る
		if (isGround_)
		{
			dogState_ = DogState::Idle;
		}
	}
}

void Dog::UpdateAnim()
{
	if (isGround_)
	{
		dogState_ = DogState::Idle;
		vel_.x = 0.0f; //Idle 時は横速度も止める
	}

	auto idleAnim = animations_[static_cast<int>(DogState::Idle)];
	auto jumpAnim = animations_[static_cast<int>(DogState::Jump)];

	// アニメーション更新
	switch (dogState_)
	{
	case DogState::Idle:
		idleAnim->Update();
		break;

	case DogState::Jump:
		if (!jumpAnim->IsAnimFinished())
		{
			jumpAnim->Update();
		}
		else
		{
			dogState_ = DogState::Idle;
		}
		break;
	}
}