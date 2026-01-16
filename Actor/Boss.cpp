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

	//被弾無敵時間
	constexpr int kHitInvincibleTime = 20;
	constexpr int kBlinkInterval = 6;
}

Boss::Boss(Vector2 pos, Vector2 vel, std::shared_ptr<Player>player, BulletManager* bm, std::shared_ptr<Camera>camera) :
	Enemy(pos, vel),
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
	isInvincible_(false),
	hitInvincibleTimer_(0),
	isHitInvincible_(false)
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

	colRect_.SetCenter(pos_.x, pos_.y,
		colSize_ * kColScale, colSize_ * kColScale);

	//アニメーションの更新
	int graphIdx = GetGraphIndex(currentState_);
	animations_[graphIdx]->Update();

	switch (currentState_)
	{
	case BossState::Idle:
		UpdateIdle();
		break;
	case BossState::Move:
		UpdateMove();
		break;
	case BossState::Attack:
		UpdateAttack();
		break;
	case BossState::Guard:
		UpdateGuard();
		break;
	case BossState::JumpAttack:
		UpdateJumpAttack();
		break;
	case BossState::Hurt:
		UpdateHurt();
		break;
	case BossState::Dead:
		UpdateDead();
		break;
	}

	//無敵時間の更新
	if (isHitInvincible_)
	{
		hitInvincibleTimer_--;
		if (hitInvincibleTimer_ <= 0)
		{
			isHitInvincible_ = false;
		}
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
	if (isHitInvincible_)
	{
		if ((hitInvincibleTimer_ / kBlinkInterval) % 2 == 0)
		{
			return; //このフレームは描画しない
		}
	}

	int graphIndex = GetGraphIndex(currentState_);

	float drawX = pos_.x + cameraOffset_.x + drawOffset_.x;
	float drawY = pos_.y + cameraOffset_.y + drawOffset_.y;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Boss::ChangeState(BossState nextState)
{
	if (!isActive_ && nextState != BossState::Idle)return;

	currentState_ = nextState;
	stateTimer_ = 0;

	//状態変更時に速度0にする
	vel_.x = 0.0f;
	if (nextState != BossState::Attack &&
		nextState != BossState::JumpAttack)
	{
		vel_.y = 0.0f;
	}

	animations_[GetGraphIndex(nextState)]->Reset();
	if (nextState != BossState::Hurt && nextState != BossState::Dead)
	{
		// Hurt 以外の状態では通常通り無敵解除
		isHitInvincible_ = true;
	}
}

void Boss::UpdateHurt()
{
	isHitInvincible_ = false;

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

	// 死亡アニメの更新
	int animIdx = GetGraphIndex(BossState::Dead);
	animations_[animIdx]->Update();

	// 落下処理（必要なら）
	if (pos_.y < kGround)
	{
		vel_.y += kGravity;
		pos_.y += vel_.y;
	}
	else
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
	}

	// 死亡アニメが終わったらフラグを立てる
	if (animations_[animIdx]->IsAnimFinished())
	{
		isDead_ = true;                 // ボスは死亡状態
		isDeadAnimFinished_ = true;     // アニメ終了
	}
}

void Boss::OnHit(int damage)
{
	//被弾中・無敵状態・死亡状態なら無視
	if (!isHitInvincible_&&currentState_==BossState::Hurt) return;

	hp_ -= damage;
	StartHitInvincible();

	// HP が 0 以下になったら死亡ステートへ
	if (hp_ <= 0 && currentState_ != BossState::Dead)
	{
		pCamera_->Shake(60, 15.0f);
		ChangeState(BossState::Dead);
		stateTimer_ = 0;
		return;
	}

	// HP が残っていれば被弾演出
	pCamera_->Shake(kCameraDuration, kCameraMagnitude);
	ChangeState(BossState::Hurt);
}

void Boss::StartHitInvincible()
{
	isHitInvincible_ = true;
	hitInvincibleTimer_ = kHitInvincibleTime;
}
