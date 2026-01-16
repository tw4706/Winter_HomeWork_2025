#include "Boss2.h"
#include"Camera.h"
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

	constexpr int kGuardTime = 120;
	constexpr int kBarrierBreakDamage = 10;

	constexpr int kShieldSrcX = 312;
	constexpr int kShieldSrcY = 216;
	constexpr int kShieldSize = 24;
	constexpr float kShieldScale = 10.0f;

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
	hp_ = kMaxHP;
	drawOffset_.y = -(kGraphH * 0.5f + 80);
}

void Boss2::Update()
{
	Boss::Update();

	if (currentState_ == BossState::Dead)
	{
		UpdateDead();
		return;
	}

	if (!isHitInvincible_)
	{
		colRect_.SetCenter(
			pos_.x,
			pos_.y - 100,
			kColSizeX,
			kColSizeY);
	}
}

void Boss2::Draw()
{
	Boss::Draw();

	int graphIndex = GetGraphIndex(currentState_);

	if (currentState_ == BossState::Guard && isBarrierActive_)
	{
		float drawX = pos_.x + cameraOffset_.x;
		float drawY = pos_.y + cameraOffset_.y;
		// 少し前に出す（向き対応）
		float offsetX = isTurn_ ? -40.0f : 40.0f;
		float offsetY = -100.0f;

		DrawRectRotaGraph3(
			static_cast<int>(drawX + offsetX),
			static_cast<int>(drawY + offsetY),
			kShieldSrcX, kShieldSrcY,
			kShieldSize, kShieldSize,
			kShieldSize / 2, kShieldSize / 2,
			kShieldScale, kShieldScale,
			0.0f,
			barrierGraphHandle_, true);
	}

#ifdef _DEBUG
	//当たり判定表示
	colRect_.DrawAndCamera(cameraOffset_, GetColor(255, 0, 0), false);

	//デバッグ用HP表示
	char buf[64];
	sprintf_s(buf, "Boss2 HP: %d / %d", hp_, kMaxHP);

	//右上表示（画面右端から文字幅分だけ左に寄せる）
	int textWidth = GetDrawStringWidth(buf, strlen(buf));
	int posX = 1280 - textWidth - 10; //右端から10px内側
	int posY = 10; //上から10px下

	DrawString(posX, posY, buf, GetColor(255, 255, 255));
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
	case BossState::Guard:
		return Anim::Idle;
	case BossState::JumpAttack:
		return Anim::Attack;
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

	pos_.y += sin(stateTimer_ * 0.3f) * 0.3f;

	if (animations_[GetGraphIndex(BossState::Hurt)]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateGuard()
{
	if (stateTimer_ == 0)
	{
		isBarrierActive_ = true;
	}

	stateTimer_++;

	if (stateTimer_ > kGuardTime)
	{
		isBarrierActive_ = false;
		ChangeState(BossState::Idle);
	}
}

void Boss2::UpdateJumpAttack()
{
	stateTimer_++;

	if (stateTimer_ < 30)
	{
		vel_.x = 0.0f; // 溜め
		return;
	}

	if (stateTimer_ == 30)
	{
		vel_.y = -20.0f;
		vel_.x = isTurn_ ? -4.0f : 4.0f;
		isJumping_ = true;
		isGround_ = false;
	}

	// 着地したら Idle に戻す
	if (isJumping_ && isGround_)
	{
		isJumping_ = false;
		if(pCamera_)
		{
			pCamera_->Shake(10, 8.0f); // 着地演出
		}
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

void Boss2::OnHit(int damage, const BulletType& type)
{
	if (isHitInvincible_) return; //無敵中は無視
	if (currentState_ == BossState::Dead) return;

	//バリア処理
	if (currentState_ == BossState::Guard && isBarrierActive_)
	{
		if (type == BulletType::Torch)
		{
			damage /= 2; //Torchだけ半減
		}

		//バリア破壊判定
		if (damage >= kBarrierBreakDamage)
		{
			isBarrierActive_ = false;
			ChangeState(BossState::Idle);
		}
		else
		{
			//バリア中はそれ以下のダメージは無効
			damage = 0;
		}
	}
	Boss::OnHit(damage);
}
