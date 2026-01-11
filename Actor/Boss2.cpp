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

	constexpr int kFrameInterval = 6;

	enum Anim
	{
		Idle,
		Attack,
		Fly,
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

	barrierHP_ = 30;
	currentWeakPoint_ = WeakPointType::Barrier;
	weakDamageRate_ = 1.0f;

	int graph = LoadGraph("data/Enemy/Boss2.png");

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
			false,
			kStartY[i]);
	}

	//èdóÕìKóp
	SetUseGravity(true);

	currentState_ = BossState::Idle;
	stateTimer_ = 0;
}

void Boss2::Update()
{
	Boss::Update();

	colRect_.SetCenter(pos_.x, pos_.y-100, kColSizeX, kColSizeY);
}

void Boss2::Draw()
{
	int graphIndex = GetGraphIndex(currentState_);

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y-150;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

int Boss2::GetGraphIndex(BossState state) const
{
	switch (state)
	{
	case BossState::Idle:
		return Anim::Idle;
	case BossState::Attack:
		return Anim::Attack;
	case BossState::Move:
		return Anim::Fly;
	case BossState::Exposed:
		return Anim::Idle;
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

	// àÍíËéûä‘Ç≈çUåÇÇ÷
	if (stateTimer_ > 60)
	{
		ChangeState(BossState::Attack);
	}
}

void Boss2::UpdateAttack()
{
}

void Boss2::UpdateMove()
{
	stateTimer_++;

	//ï‡çs
	vel_.x = isTurn_ ? -1.5f : 1.5f;

	//àÍíËéûä‘Ç≈IdleÇ÷
	if (stateTimer_ > 60)
	{
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateExposed()
{
	stateTimer_++;

	//ñ≥ñhîıíÜÇÕñ≥ìGâèú
	isInvincible_ = false;

	//àÍíËéûä‘Ç≈çƒçsìÆ
	if (stateTimer_ > 90)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateHurt()
{
	stateTimer_++;

	//ÉmÉbÉNÉoÉbÉN
	pos_.x += isTurn_ ? 2.0f : -2.0f;
	pos_.y += sinf(stateTimer_ * 0.3f) * 0.3f;

	if (stateTimer_ > 30){}
}

void Boss2::OnHit(int damage)
{
	//îÌíeíÜÅEñ≥ìGèÛë‘Ç»ÇÁñ≥éãÇ∑ÇÈ
	if (isHitInvincible_) return;

	//HurtíÜÅEéÄñSíÜÇÕñ≥éãÇ∑ÇÈ
	if (currentState_ == BossState::Hurt ||currentState_ == BossState::Dead)return;

	Boss::OnHit(damage);

	if (currentState_ != BossState::Dead)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::SelectWeakPoint()
{
	int r = GetRand(2); // 0Å`2

	currentWeakPoint_ = static_cast<WeakPointType>(r);

	switch (currentWeakPoint_)
	{
	case WeakPointType::BarrierCore:
		barrierHP_ = 30;
		weakDamageRate_ = 2.0f;
		break;

	case WeakPointType::CenterCore:
		weakDamageRate_ = 2.0f;
		break;

	case WeakPointType::GroundCore:
		weakDamageRate_ = 1.5f;
		break;
	}
}
