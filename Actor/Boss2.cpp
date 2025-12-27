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
}

void Boss2::Update()
{
	Boss::Update();

	colRect_.SetCenter(pos_.x, pos_.y-100, kColSizeX, kColSizeY);
}

void Boss2::Draw()
{
	int graphIndex = GetGraphIndex(currentState_);

	const float imageHeight = kGraphH * kScale;

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y-180;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Boss2::LoadResources()
{
}

int Boss2::GetGraphIndex(BossState state) const
{
	return 0;
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

void Boss2::SetAttackMode(BossAttackMode mode)
{
	bossMode_ = mode;
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
		vel_.x = isTurn_ ? -5.0f : 5.0f;
	}
	stateTimer_++;

	if (stateTimer_ > 60)
	{
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}

void Boss2::AttackTorch()
{
	if (stateTimer_ == 0 && isGround_)
	{
		vel_.y = -12.0f; //小ジャンプ
	}

	stateTimer_++;

	if (stateTimer_ > 150)
	{
		ChangeState(BossState::Idle);
	}
}
