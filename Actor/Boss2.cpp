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

	//重力適用
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

	// 一定時間で攻撃へ
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

	//歩行
	vel_.x = isTurn_ ? -1.5f : 1.5f;

	//一定時間でIdleへ
	if (stateTimer_ > 60)
	{
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateExposed()
{
	stateTimer_++;

	//無防備中は無敵解除
	isInvincible_ = false;

	//一定時間で再行動
	if (stateTimer_ > 90)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateHurt()
{
	stateTimer_++;

	//ノックバック
	pos_.x += isTurn_ ? 2.0f : -2.0f;
	pos_.y += sinf(stateTimer_ * 0.3f) * 0.3f;

	if (stateTimer_ > 30){}
}

void Boss2::OnHit(int damage)
{
	//被弾中・無敵状態なら無視する
	if (isHitInvincible_) return;

	//Hurt中・死亡中は無視する
	if (currentState_ == BossState::Hurt ||currentState_ == BossState::Dead)return;

	Boss::OnHit(damage);

	//槍の場合はすぐに戻る
	if (currentState_ != BossState::Dead)
	{
		ChangeState(BossState::Idle);
	}
}
