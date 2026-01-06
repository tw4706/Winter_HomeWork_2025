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
		kGraphH * 0, // Idle
		kGraphH * 1, // Attack
		kGraphH * 2, // Fly
		kGraphH * 3, // Hurt
		kGraphH * 4  // Dead
	};

	constexpr int kFrameCount[kAnimNum] =
	{
		6,	//Idle
		10, //Attack
		14, //Fly
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

	//起動前も地面にいさせる
	SetUseGravity(true);

	currentState_ = BossState::Idle;
	stateTimer_ = 0;
}

void Boss2::Update()
{
	if (isActive_ && !isModeDecided_)
	{
		DecideAttackMode(pPlayer_->GetCurrentBulletType());
		isModeDecided_ = true;
	}
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

bool Boss2::IsDamageable() const
{
	//ダメージを与えられる状態だけTrueを返す
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

void Boss2::SetPlayerWeapon(BulletType weapon)
{
	playerWeapon_ = weapon;
}

void Boss2::DecideAttackMode(BulletType playerWeapon)
{
	switch (playerWeapon)
	{
	case BulletType::Knife:
		bossMode_ = BossAttackMode::Knife;
		hp_ = 50;
		break;
	case BulletType::Lance:
		bossMode_ = BossAttackMode::Lance;
		hp_ = 30;
		break;
	case BulletType::Torch:
		bossMode_ = BossAttackMode::Torch;
		hp_ = 40;
		break;
	default:
		bossMode_ = BossAttackMode::Knife;
		break;
	}
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

	if (stateTimer_ > 30)
	{
		switch (bossMode_)
		{
		case BossAttackMode::Lance:
			ChangeState(BossState::Exposed);
			break;
		default:
			ChangeState(BossState::Idle);
			break;
		}
	}
}

void Boss2::OnHit(int damage)
{
	//被弾中・無敵状態なら無視する
	if (isHitInvincible_) return;

	//Hurt中・死亡中は無視する
	if (currentState_ == BossState::Hurt ||currentState_ == BossState::Dead)return;

	//ダメージが通らない状態でもHurt状態へ移行
	if (!IsDamageable())
	{
		ChangeState(BossState::Hurt);
		return;
	}

	Boss::OnHit(damage);

	//槍の場合はすぐに戻る
	if (bossMode_ == BossAttackMode::Lance &&
		currentState_ != BossState::Dead)
	{
		ChangeState(BossState::Idle);
	}
}


void Boss2::AttackKnife()
{
	//ナイフの場合は左右に揺れ動く
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
		//大ジャンプ
		vel_.y = -15.0f;
	}

	stateTimer_++;

	vel_.x = isTurn_ ? -4.0f : 4.0f;

	//一定時間で停止
	if (isGround_ && stateTimer_ > 30)
	{
		vel_ = { 0.0f, 0.0f };
		isInvincible_ = false;
		//ダメージチャンス
		ChangeState(BossState::Exposed);
	}
}

void Boss2::AttackTorch()
{
	stateTimer_++;

	//空中は無敵
	isInvincible_ = !isGround_;

	if (stateTimer_ == 1 && isGround_)
	{
		vel_.y = -12.0f;
	}

	vel_.x = isTurn_ ? -2.5f : 2.5f;

	//着地後の隙
	if (isGround_ && stateTimer_ > 40)
	{
		isInvincible_ = false;
		vel_.x = 0.0f;
		ChangeState(BossState::Idle);
	}
}
