#include "Boss2.h"
#include<Dxlib.h>

namespace
{
	constexpr int kGraphW = 192;
	constexpr int kGraphH = 108;
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
		kGraphH * 0, // Idle
		kGraphH * 1, // Attack
		kGraphH * 2, // Fly
		kGraphH * 3, // Hurt
		kGraphH * 4  // Dead
	};

	constexpr int kFrameCount[kAnimNum] =
	{
		6, // Idle
		10, // Attack
		14, // Fly
		7, // Hurt
		16  // Dead
	};
}


void Boss2::Init()
{
	//スプライトシートを1枚だけ読み込む
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

	SetUseGravity(true);

	currentState_ = BossState::Idle;
	stateTimer_ = 0;
	bossMode_ = BossAttackMode::Knife;
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
	float drawY = pos_.y + cameraOffset_.y;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

bool Boss2::IsDamageable() const
{
	//ダメージを与えられる状態だけtrueを返す
	switch (bossMode_)
	{
	case BossAttackMode::Knife:
		return true;
	case BossAttackMode::Lance:
		return currentState_ == BossState::Exposed;
	case BossAttackMode::Torch:
		return currentState_ == BossState::Idle;
	default:
		return true;
	}
}

void Boss2::LoadResources()
{
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
	switch (bossMode_)
	{
	case BossAttackMode::Knife:
		AttackKnife();
		break;
	case BossAttackMode::Lance:
		AttackLance();
		break;
	case BossAttackMode::Torch:
		AttackTorch();
		break;
	}
}

void Boss2::UpdateMove()
{

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

	// 無敵点滅用
	isInvincible_ = true;

	if (stateTimer_ > 30)
	{
		isInvincible_ = false;

		switch (bossMode_)
		{
		case BossAttackMode::Knife:
			ChangeState(BossState::Idle);
			break;
		case BossAttackMode::Lance:
			ChangeState(BossState::Exposed);
			break;
		case BossAttackMode::Torch:
			ChangeState(BossState::Idle);
			break;
		}
	}
}

void Boss2::OnHit(int damage)
{
	//Hurt中・死亡中は無視
	if (currentState_ == BossState::Hurt ||currentState_ == BossState::Dead)return;

	//ダメージが通らない状態でもHurt状態へ移行
	if (!IsDamageable())
	{
		ChangeState(BossState::Hurt);
		return;
	}

	Boss::OnHit(damage);
}

void Boss2::SetAttackMode(BulletType type)
{
	switch (type)
	{
	case BulletType::Knife:
		bossMode_ = BossAttackMode::Knife;
		break;
	case BulletType::Lance:
		bossMode_ = BossAttackMode::Lance;
		break;
	case BulletType::Torch:
		bossMode_ = BossAttackMode::Torch;
		break;
	default:
		break;
	}
}

void Boss2::AttackKnife()
{
	//ナイフ攻撃：左右に揺れ動く
	vel_.x = sinf(stateTimer_ * 0.15f) * 1.5f;
	stateTimer_++;

	if (stateTimer_ > 90)
	{
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}

void Boss2::AttackLance()
{
	if (stateTimer_ == 0)
	{
		isInvincible_ = true;
		vel_.x = isTurn_ ? -5.0f : 5.0f;
	}

	stateTimer_++;

	if (stateTimer_ > 60)
	{
		vel_.x = 0.0f;
		isInvincible_ = false;
		ChangeState(BossState::Exposed);
	}
}

void Boss2::AttackTorch()
{
	isInvincible_ = false;

	if (stateTimer_ == 0 && isGround_)
	{
		vel_.y = -14.0f;
		vel_.x = isTurn_ ? -3.0f : 3.0f;
	}

	stateTimer_++;

	//着地後だけ隙
	if (isGround_ && stateTimer_ > 50)
	{
		ChangeState(BossState::Idle);
	}
}
