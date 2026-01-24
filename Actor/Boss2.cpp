#include "Boss2.h"
#include"Camera.h"
#include"SEManager.h"
#include"Application.h"
#include"BulletManager.h"
#include<Dxlib.h>

namespace
{
	constexpr int kGraphW = 192;
	constexpr int kGraphH = 128;
	constexpr float kScale = 4.0f;
	constexpr float kColSizeX = 128.0f;
	constexpr float kColSizeY = 256.0f;

	constexpr int kMaxHP = 50;

	constexpr int kShieldSrcX = 312;
	constexpr int kShieldSrcY = 216;
	constexpr int kShieldSize = 24;
	constexpr float kShieldScale = 10.0f;

	constexpr int kShieldMaxHP = 30;
	constexpr int kShieldBreakTime = 180;

	constexpr int   kShieldReactTime = 20;   //ヒット反応保持時間
	constexpr float kShieldFadeSpeed = 10.0f;

	constexpr int kFrameInterval = 10;
	constexpr int kFrameIntervalDead = 25;

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
	colRect_.SetCenter(pos_.x, pos_.y - 100, kColSizeX, kColSizeY);

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

	float offsetX = isTurn_ ? -40.0f : 40.0f;
	float offsetY = -100.0f;
	float shieldRate = (float)shieldHP_ / shieldMaxHP_;

	float shieldScale = kShieldScale * (0.6f + shieldRate * 0.4f);
	bool isShieldCritical = (shieldRate <= 0.3f);

	//シールドの揺れを残量によって変化させる
	float shakeX = 0.0f;
	float shakeY = 0.0f;

	if (isShieldCritical && !isShieldBroken_)
	{
		shakeX = (GetRand(5) - 2) * 2.0f;
		shakeY = (GetRand(5) - 2) * 2.0f;
	}

	if (shieldHitTimer_ > 0)
	{
		int alpha = isShieldCritical ? 200 : 128;
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
	if (stateTimer_ > 30)
	{
		UpdateJumpAttack();
	}
}

void Boss2::UpdateMove()
{
	stateTimer_++;

	//歩行
	vel_.x = isTurn_ ? -1.5f : 1.5f;

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

	if (stateTimer_ < 30)
	{
		vel_.x = 0.0f; //溜め
		return;
	}

	if (stateTimer_ == 30 && isGround_)
	{
		vel_.y = -20.0f;
		vel_.x = isTurn_ ? -4.0f : 4.0f;
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
			pCamera_->Shake(10, 8.0f); //着地演出
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
			shieldDamage = 2;
			break;

		case BulletType::Lance:
			shieldDamage = 5;
			break;

		case BulletType::Torch:
			shieldDamage = 3;
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

			//破壊ボーナスダメージ
			Boss::OnHit(10);
			return;
		}

		//まだシールドがあるなら本体は無傷
		return;
	}

	//シールドが無いときは普通にダメージ
	Boss::OnHit(damage);
}
