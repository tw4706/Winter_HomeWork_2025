#include "Boss2.h"
#include<Dxlib.h>

namespace
{
	constexpr int kGraphW = 192;
	constexpr int kGraphH = 108;
	constexpr float kScale = 1.0f;

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
		4, // Idle
		8, // Attack
		4, // Fly
		4, // Hurt
		7  // Dead
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

	currentState_ = BossState::Idle;
	stateTimer_ = 0;
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
	switch (lastHitBulletType_)
	{
	case BulletType::Knife:
		AttackKnife();
		break;
	case BulletType::Lance:
		AttackLance();
		break;
	case BulletType::Torch:
		AttackTorch();
		break;
	default:
		break;
	}
}

void Boss2::UpdateMove()
{

}

void Boss2::AttackKnife()
{
	stateTimer_++;

	pos_.x += sinf(stateTimer_ * 0.15f) * 1.5f;

	if (stateTimer_ > 90)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::AttackLance()
{
	if (stateTimer_ == 0)
	{
		vel_.x = isTurn_ ? -5.0f : 5.0f;
	}

	pos_.x += vel_.x;
	stateTimer_++;

	if (stateTimer_ > 60)
	{
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}

void Boss2::AttackTorch()
{
	pos_.y = backPos_.y - 100.0f + sinf(stateTimer_ * 0.05f) * 20.0f;
	stateTimer_++;

	if (stateTimer_ > 150)
	{
		ChangeState(BossState::Idle);
	}
}
