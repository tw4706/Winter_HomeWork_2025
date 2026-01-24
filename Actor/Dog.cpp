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

	const int frameCounts[kGraphNum] = { 11, 6 };
	const int frameIntervals[kGraphNum] = { 8, 6 };

	//画像の切り取りサイズ
	constexpr int kGraphW = 64;
	constexpr int kGraphH = 48;
	constexpr int kGraphColSize = 48;
	constexpr float kScale = 2.0f;

	constexpr float kDrawW = kGraphW * 2.0f;
	constexpr float kDrawH = kGraphH * 2.0f;

	//ジャンプの高さ
	constexpr float kJumpPower = 15.0f;
	//移動速度
	constexpr float kSpeed = 4.0f;
	//ジャンプ間隔
	constexpr int kJumpInterval = 60;
	//プレイヤーとの距離
	const float kDistance = 300.0f;
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
	colRect_.SetCenter(pos_.x, pos_.y-5, kGraphColSize, kGraphColSize);

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
	float drawY = (pos_.y + cameraOffset_.y)-30;

	switch (dogType_)
	{
	case DogType::Normal:
		break;
	case DogType::Weak:
		break;
	default:
		break;
	}

	animations_[static_cast<int>(dogState_)]->Draw(drawX, drawY, isTurn_);

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