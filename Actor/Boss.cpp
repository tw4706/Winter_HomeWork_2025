#include "Boss.h"
#include "Player.h"
#include"Camera.h"
#include "BulletManager.h"
#include<Dxlib.h>
#include<cassert>
#include<string>

namespace
{
	constexpr float kColScale = 2.0f;
	constexpr float kColSize = 64.0f;

	constexpr float kActiveDistance = 500.0f;
	constexpr float kShotInterval = 5.0f;
	constexpr int KMaxHp = 30;

	//地形・距離関連
	constexpr float kGravity = 0.5f;
	constexpr float kDistance = 400.0f;
	constexpr float kGround = 1764.0f;

	//カメラの揺れ関連
	constexpr int kCameraDuration = 10;
	constexpr int kCameraMagnitude = 8;
}

Boss::Boss(Vector2 pos, Vector2 vel, std::shared_ptr<Player>player, BulletManager*bm,std::shared_ptr<Camera>camera):
	Enemy(pos,vel),
	pPlayer_(player),
	pBm_(bm),
	pCamera_(camera),
	currentState_(BossState::Idle),
	stateTimer_(0),
	shotTimer_(0.0f),
	shotInterval_(kShotInterval),
	hp_(KMaxHp),
	backPos_(pos),
	hasShot_(false),
	isActive_(false),
	isCharging_(false),
	isInvincible_(false)
{
	colSize_ = kColSize;
}

Boss::~Boss()
{
}

void Boss::Init()
{
	LoadResources();
	currentState_ = BossState::Idle;
	stateTimer_ = 0;
}

void Boss::Update()
{
	float distance = std::abs(pPlayer_->GetPos().x - pos_.x);

	//プレイヤーが一定距離まで近づいたら起動
	if (!isActive_)
	{
		if (distance < kActiveDistance)
		{
			isActive_ = true;
			stateTimer_ = 0;
		}
		else
		{
			colRect_.SetCenter(pos_.x, pos_.y, 0, 0);

			//Idleアニメだけ更新
			animations_[GetGraphIndex(BossState::Idle)]->Update();

			Enemy::Update();
			return;
		}
	}

	//常にプレイヤーの方向を向く
	isTurn_ = (pPlayer_->GetPos().x < pos_.x);

	colRect_.SetCenter(pos_.x-20, pos_.y, colSize_ * kColScale, colSize_ * kColScale);

	//アニメーションの更新
	int graphIdx = GetGraphIndex(currentState_);
	animations_[graphIdx]->Update();

	switch (currentState_)
	{
	case BossState::Idle:
		UpdateIdle();
		break;
	case BossState::Attack:
		UpdateAttack();
		break;
	case BossState::Move:
		UpdateMove();
		break;
	case BossState::Exposed:
		UpdateExposed();
		break;
	case BossState::Hurt:
		UpdateHurt();
		break;
	case BossState::Dead:
		UpdateDead();
		break;
	}

	Enemy::Update();

#ifdef _DEBUG
	//Kキーで即死
	if (CheckHitKey(KEY_INPUT_K))
	{
		hp_ = 0;
		pCamera_->Shake(60, 15.0f);
		ChangeState(BossState::Dead);
		return;
	}
#endif

}

void Boss::Draw()
{
	int graphIndex = GetGraphIndex(currentState_);

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Boss::ChangeState(BossState nextState)
{
	isInvincible_ = false;

	if (!isActive_ && nextState != BossState::Idle)return;

	currentState_ = nextState;
	stateTimer_ = 0;

	//状態変更時に速度0にする
	vel_.x = 0.0f;
	if (nextState != BossState::Attack)
	{
		vel_.y = 0.0f;
	}

	animations_[GetGraphIndex(nextState)]->Reset();
}

void Boss::UpdateHurt()
{
    stateTimer_++;

    // 演出だけ
    pos_.y += sin(stateTimer_ * 0.3f) * 0.3f;

	int animIndex = GetGraphIndex(BossState::Hurt);
	if (animations_[animIndex]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateDead()
{
	stateTimer_++;
	vel_.y += kGravity; //重力
	pos_.y += vel_.y;

	if (pos_.y > kGround)
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
	}

	//死亡アニメーションが終わったらガチの死亡
	int animIdx = GetGraphIndex(BossState::Dead);
	if (stateTimer_ > 0 && animations_[animIdx]->IsAnimFinished())
	{
		isDead_ = true;
	}
}

void Boss::OnHit(int damage)
{
	if (isInvincible_) return;
	if (currentState_ == BossState::Dead) return;

	hp_ -= damage;

	//体力が0以下ならカメラを大きく揺らして死亡状態へ
	if (hp_ <= 0)
	{
		pCamera_->Shake(60, 15.0f);
		ChangeState(BossState::Dead);
		return;
	}

	//カメラを揺らす
	pCamera_->Shake(kCameraDuration, kCameraMagnitude);
	ChangeState(BossState::Hurt);
}
