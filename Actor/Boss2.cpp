#include "Boss2.h"
#include"BulletManager.h"
#include<Dxlib.h>

namespace
{
	constexpr int kGraphW = 192;
	constexpr int kGraphH = 128;
	constexpr float kScale = 4.0f;
	constexpr float kColSizeX = 128.0f;
	constexpr float kColSizeY = 256.0f;

	constexpr int kBarrierHP = 20;
	constexpr int kBarrierBreakDamage = 10;

	constexpr int kFrameInterval = 6;

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
		kGraphH * 1, //Attack
		kGraphH * 2, //Move
		kGraphH * 3, //Hurt
		kGraphH * 4  //Dead
	};

	constexpr int kFrameCount[kAnimNum] =
	{
		6,	//Idle
		10, //Attack
		14, //Move
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

		animations_[i] = std::make_shared<Animation>(
			graph,
			kGraphW,
			kGraphH,
			kFrameCount[i],
			kFrameInterval,
			kScale,
			(i != Anim::Hurt && i != Anim::Dead),
			kStartY[i]);
	}

	//重力適用
	SetUseGravity(true);

	currentState_ = BossState::Idle;
	stateTimer_ = 0;
}

void Boss2::Update()
{
	Boss::Update();

	colRect_.SetCenter(pos_.x, pos_.y - 100, kColSizeX, kColSizeY);
}

void Boss2::Draw()
{
	int graphIndex = GetGraphIndex(currentState_);

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y - 150;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

	if (currentState_ == BossState::Guard && isBarrierActive_)
	{
		// 少し前に出す（向き対応）
		float offsetX = isTurn_ ? -40.0f : 40.0f;

		DrawRotaGraph(
			static_cast<int>(drawX + offsetX),
			static_cast<int>(drawY),
			1.0,
			0.0,
			barrierGraphHandle_,
			TRUE
		);
	}

#ifdef _DEBUG
	//当たり判定表示
	colRect_.DrawAndCamera(cameraOffset_, GetColor(255, 0, 0), false);
#endif
}

int Boss2::GetGraphIndex(BossState state) const
{
	switch (state)
	{
	case BossState::Idle:
		return Anim::Idle;
	case BossState::Move:
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
	if (stateTimer_ > 60)
	{
		DecideAttack();
	}
}

void Boss2::UpdateAttack()
{
	stateTimer_++;

	//攻撃終了判定
	if (stateTimer_ > 90)
	{
		ChangeState(BossState::Idle);
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

	if (stateTimer_ > 30)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateGuard()
{
	if (stateTimer_ == 0)
	{
		isBarrierActive_ = true;
		barrierHp_ = kBarrierHP;
	}

	stateTimer_++;
	vel_.x = 0.0f;
}

void Boss2::UpdateJumpAttack()
{
	stateTimer_++;

	// 溜め
	if (stateTimer_ < 30)
	{
		vel_.x = 0.0f;
		return;
	}

	// ジャンプ開始
	if (stateTimer_ == 30)
	{
		vel_.y = -8.0f;
		vel_.x = isTurn_ ? -3.0f : 3.0f;
	}

	// 着地
	if (isGround_)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::DecideAttack()
{
	stateTimer_ = 0;

	int r = GetRand(1);

	if (r == 0)
	{
		attackType_ = Boss2AttackType::Barrier;
		ChangeState(BossState::Guard);
	}
	else
	{
		attackType_ = Boss2AttackType::JumpAttack;
		ChangeState(BossState::JumpAttack);
	}
}

void Boss2::OnHit(int damage)
{
	if (currentState_ == BossState::Guard &&
		attackType_ == Boss2AttackType::Barrier &&
		isBarrierActive_)
	{
		barrierHp_ -= damage;
		if (barrierHp_ <= 0)
		{
			isBarrierActive_ = false;

			Boss::OnHit(kBarrierBreakDamage);
			ChangeState(BossState::Hurt);
		}
		return;
	}
	Boss::OnHit(damage);

	if (currentState_ != BossState::Dead)
	{
		ChangeState(BossState::Hurt);
	}
}
