#include "Boss2.h"
#include"Camera.h"
#include"SEManager.h"
#include"Application.h"
#include"BulletManager.h"
#include<Dxlib.h>

namespace
{
	//========================
	// ステータス関連
	//========================
	constexpr int kMaxHP = 50;

	//========================
	// 当たり判定・描画関連
	//========================
	constexpr float kColOffsetY = 100.0f;
	constexpr float kShieldOffsetX = 40.0f;
	constexpr float kShieldOffsetY = 100.0f;
	constexpr float kDrawOffsetExtraY = 80.0f;
	constexpr int kGraphW = 192;
	constexpr int kGraphH = 128;
	constexpr float kScale = 4.0f;
	constexpr float kColSizeX = 128.0f;
	constexpr float kColSizeY = 256.0f;

	//========================
	// シールド関連
	//========================
	constexpr int kShieldSrcX = 312;
	constexpr int kShieldSrcY = 216;
	constexpr int kShieldSize = 24;
	constexpr float kShieldScale = 10.0f;
	constexpr int kShieldMaxHP = 30;
	constexpr int kShieldBreakTime = 180;
	constexpr int   kShieldReactTime = 20;   //ヒットした時の反応する時間
	constexpr float kShieldFadeSpeed = 10.0f;
	constexpr float kShieldCriticalRate = 0.3f;
	constexpr float kShieldScaleMinRate = 0.6f;
	constexpr float kShieldScaleMaxRate = 0.4f;
	constexpr int   kShieldShakeRand = 5;
	constexpr float kShieldShakePower = 2.0f;
	constexpr int   kShieldHitAlphaNormal = 128;
	constexpr int   kShieldHitAlphaCritical = 200;

	//========================
	// 状態遷移関連
	//========================
	constexpr int kIdleToJumpTime = 30;
	constexpr int kMoveDuration = 60;
	constexpr int kJumpChargeTime = 30;

	//========================
	// 移動関連
	//========================
	constexpr float kMoveSpeed = 1.5f;
	constexpr float kJumpSpeedY = 20.0f;
	constexpr float kJumpSpeedX = 4.0f;

	//========================
	// 被弾演出関連
	//========================
	constexpr float kHurtShakeSpeed = 0.3f;
	constexpr float kHurtShakePower = 0.3f;

	//========================
	// 着地演出関連
	//========================
	constexpr int   kCameraDuration = 10;
	constexpr float kCameraMagnitude = 8.0f;

	//========================
	// シールドへのダメージ関連
	//========================
	constexpr int kKnifeShieldDamage = 2;
	constexpr int kLanceShieldDamage = 5;
	constexpr int kTorchShieldDamage = 3;
	constexpr int kShieldBreakDamage = 10;

	enum Anim
	{
		Idle,
		Attack,
		Move,
		Hurt,
		Dead,
		kAnimNum
	};

	constexpr int kStartY[kAnimNum] =
	{
		kGraphH * 0, //Idle
		kGraphH * 2, //Attack
		kGraphH * 1, //Move
		kGraphH * 3, //Hurt
		kGraphH * 4  //Dead
	};

	constexpr int kFrameCount[kAnimNum] =
	{
		6,	//Idle
		14, //Attack
		10, //Move
		7,	//Hurt
		16  //Dead
	};
	constexpr int kFrameInterval = 10;
	constexpr int kFrameIntervalDead = 25;
}

void Boss2::Init()
{
	Boss::Init();

	int graph = LoadGraph("data/Enemy/Boss2.png");
	barrierGraphHandle_ = LoadGraph("data/UI/Icon.png");

	graphHandles_.resize(kAnimNum);
	animations_.resize(kAnimNum);

	for (int i = 0; i < kAnimNum; i++)
	{
		graphHandles_[i] = graph;

		int frameInterval = (i == Anim::Dead) ? kFrameIntervalDead : kFrameInterval;

		animations_[i] = std::make_shared<Animation>(
			graph,
			kGraphW,
			kGraphH,
			kFrameCount[i],
			frameInterval,
			kScale,
			(i != Anim::Hurt && i != Anim::Dead),
			kStartY[i]);
	}

	//重力適用
	SetUseGravity(true);

	currentState_ = BossState::Idle;
	stateTimer_ = 0;
	hp_ = kMaxHP;
	drawOffset_.y = -(kGraphH * 0.5f + 80);
	isBarrierActive_ = true;
	isShieldBroken_ = false;
	shieldMaxHP_ = kShieldMaxHP;
	shieldHP_ = shieldMaxHP_;
	shieldBreakTimer_ = 0;

	shieldAlpha_ = 0.0f;
	shieldHitTimer_ = 0;
}

void Boss2::Update()
{
	Boss::Update();

	if (currentState_ == BossState::Dead)
	{
		UpdateDead();
		return;
	}
	colRect_.SetCenter(pos_.x, pos_.y - kColOffsetY, kColSizeX, kColSizeY);

	//シールドの更新
	if (shieldHitTimer_ > 0)
	{
		shieldHitTimer_--;
	}

	//シールドの破壊判定
	if (isShieldBroken_)
	{
		shieldBreakTimer_--;
		if (shieldBreakTimer_ <= 0)
		{
			//シールド復活
			isShieldBroken_ = false;
			isBarrierActive_ = true;
			shieldHP_ = shieldMaxHP_;
		}
	}
}

void Boss2::Draw()
{
	Boss::Draw();

	if (isShieldBroken_)return;

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	float offsetX = isTurn_ ? -kShieldOffsetX : kShieldOffsetX;
	float offsetY = -kShieldOffsetY;
	float shieldRate = (float)shieldHP_ / shieldMaxHP_;

	float shieldScale = kShieldScale * (kShieldScaleMinRate + shieldRate * kShieldScaleMaxRate);
	bool isShieldCritical = (shieldRate <= kShieldCriticalRate);

	//シールドの揺れを残量によって変化させる
	float shakeX = 0.0f;
	float shakeY = 0.0f;

	if (isShieldCritical && !isShieldBroken_)
	{
		shakeX = (GetRand(kShieldShakeRand) - (kShieldShakeRand / 2)) * kShieldShakePower;
		shakeY = (GetRand(kShieldShakeRand) - (kShieldShakeRand / 2)) * kShieldShakePower;
	}

	if (shieldHitTimer_ > 0)
	{
		int alpha = isShieldCritical ? kShieldHitAlphaCritical : kShieldHitAlphaNormal;
		SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
	}

	DrawRectRotaGraph3(
		(int)(drawX + offsetX + shakeX),
		(int)(drawY + offsetY + shakeY),
		kShieldSrcX, kShieldSrcY,
		kShieldSize, kShieldSize,
		kShieldSize / 2, kShieldSize / 2,
		shieldScale, shieldScale,
		0.0f,
		barrierGraphHandle_, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#ifdef _DEBUG
	//当たり判定表示
	colRect_.DrawAndCamera(cameraOffset_, GetColor(255, 0, 0), false);
	char buf2[64];
	sprintf_s(buf2, "Shield HP: %d / %d", shieldHP_, shieldMaxHP_);
	DrawString(10, 30, buf2, GetColor(0, 255, 255));
#endif
}

void Boss2::OnHit(int damage)
{
	OnHit(damage, BulletType::Knife);
}

int Boss2::GetGraphIndex(BossState state) const
{
	switch (state)
	{
	case BossState::Idle:
		return Anim::Idle;
	case BossState::Move:
		return Anim::Move;
	case BossState::JumpAttack:
		return Anim::Move;
	case BossState::Hurt:
		return Anim::Hurt;
	case BossState::Dead:
		return Anim::Dead;
	default:
		return Anim::Idle;
	}
}

void Boss2::UpdateIdle()
{
	stateTimer_++;

	//一定時間で攻撃へ
	if (stateTimer_ > kIdleToJumpTime)
	{
		ChangeState(BossState::JumpAttack);
	}
}

void Boss2::UpdateMove()
{
	stateTimer_++;

	//歩行
	vel_.x = isTurn_ ? -kMoveSpeed : kMoveSpeed;

	//一定時間でIdleへ
	if (stateTimer_ > 60)
	{
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateHurt()
{
	stateTimer_++;

	pos_.y += sin(stateTimer_ * 0.3f) * 0.3f;

	if (animations_[GetGraphIndex(BossState::Hurt)]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateJumpAttack()
{
	stateTimer_++;

	if (stateTimer_ < kJumpChargeTime)
	{
		vel_.x = 0.0f; //溜め
		return;
	}

	if (stateTimer_ == kJumpChargeTime && isGround_)
	{
		vel_.y = -kJumpSpeedY;
		vel_.x = isTurn_ ? -kJumpSpeedX : kJumpSpeedX;
		isJumping_ = true;
		isGround_ = false;
	}

	//着地したら Idle に戻す
	if (isJumping_ && isGround_)
	{
		isJumping_ = false;
		isBarrierActive_ = true;
		if (pCamera_)
		{
			pCamera_->Shake(kCameraDuration, kCameraMagnitude); //着地演出
		}
		ChangeState(BossState::Idle);
	}
}

void Boss2::OnHit(int damage, const BulletType& type)
{
	if (currentState_ == BossState::Dead) return;

	//シールドが有効な場合
	if (isBarrierActive_ && !isShieldBroken_)
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::BossGuard);
		shieldHitTimer_ = kShieldReactTime;

		int shieldDamage = 0;

		switch (type)
		{
		case BulletType::Knife:
			shieldDamage = kKnifeShieldDamage;
			break;

		case BulletType::Lance:
			shieldDamage = kLanceShieldDamage;
			break;

		case BulletType::Torch:
			shieldDamage = kTorchShieldDamage;
			break;
		}

		shieldHP_ -= shieldDamage;

		//シールド破壊
		if (shieldHP_ <= 0)
		{
			isShieldBroken_ = true;
			isBarrierActive_ = false;
			shieldBreakTimer_ = kShieldBreakTime;

			Application::GetInstance().GetSEManager().PlaySE(SE::BossGuardBreak);

			//破壊で大ダメージ
			Boss::OnHit(kShieldBreakDamage);
			return;
		}

		//まだシールドがあるなら本体は無傷
		return;
	}

	//シールドが無いときは普通にダメージ
	Boss::OnHit(damage);
}
