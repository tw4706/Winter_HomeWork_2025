#include "Boss.h"
#include "Player.h"
#include"Camera.h"
#include"Application.h"
#include"SpriteEffect.h"
#include "BulletManager.h"
#include "EffectManager.h"
#include<Dxlib.h>
#include<cassert>
#include<string>

namespace
{
	constexpr float kColScale = 2.0f;
	constexpr float kColSize = 64.0f;

	constexpr float kActiveDistance = 500.0f;
	constexpr float kShotInterval = 5.0f;
	constexpr int KMaxHp = 5;

	//地形・距離関連
	constexpr float kGravity = 0.5f;
	constexpr float kDistance = 400.0f;
	constexpr float kGround = 1764.0f;

	//カメラの揺れ関連
	constexpr int kCameraDuration = 10;
	constexpr int kCameraMagnitude = 8;

	//被弾無敵時間
	constexpr int kHitInvincibleTime = 60;
	constexpr int kBlinkInterval = 6;
}

Boss::Boss(Vector2 pos, Vector2 vel,
	std::shared_ptr<Player>player, BulletManager* bm,
	std::shared_ptr<Camera>camera, EffectManager* effectMgr) :
	Enemy(pos, vel),
	currentState_(BossState::Idle),
	stateTimer_(0),
	shotTimer_(0.0f),
	shotInterval_(kShotInterval),
	backPos_(pos),
	hasShot_(false),
	isActive_(false),
	isCharging_(false),
	isInvincible_(false),
	hitInvincibleTimer_(0),
	isHitInvincible_(false),
	isDeadAnimFinished_(false),
	explosionSEHandle_(-1),
	bossDeathSEHandle_(-1)
{
	pPlayer_ = player;
	pBm_ = bm;
	pCamera_ = camera;
	pEffectManager_ = effectMgr;
	hp_ = KMaxHp;
	colSize_ = kColSize;
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

			if (currentState_ != BossState::Dead)
			{
				Enemy::Update();
			}
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
	if (currentState_ != BossState::Dead)
	{
		if (isHitInvincible_)
		{
			if ((hitInvincibleTimer_ / kBlinkInterval) % 2 == 0)
			{
				return;
			}
		}
	}

	int graphIndex = GetGraphIndex(currentState_);

	float drawX = pos_.x + cameraOffset_.x + drawOffset_.x;
	float drawY = pos_.y + cameraOffset_.y + drawOffset_.y;

	animations_[graphIndex]->Draw(drawX, drawY, !isTurn_);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);

	DrawFormatString(50, 80, GetColor(255, 255, 0),
		"State=%d Dead=%d DeadAnimFin=%d",
		(int)currentState_,
		isDead_,
		isDeadAnimFinished_);
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

	//死亡アニメーションの時は演出があるためリセットしないようにする
	if (nextState != BossState::Dead)
	{
		animations_[GetGraphIndex(nextState)]->Reset();
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
	// 一度だけ死亡演出
	if (!isPlayingDeathEffect_)
	{
		int seHandle = Application::GetInstance().GetSEManager().GetHandle(SE::Explosion);
		PlaySoundMem(seHandle, DX_PLAYTYPE_BACK, FALSE);

		if (pEffectManager_)
			pEffectManager_->AddEffect(
				std::make_shared<SpriteEffect>(
					pos_, "data/Effect/enemy_explosion.png",
					176, 16, 16, 16, 3, 4, 3.0f));

		isPlayingDeathEffect_ = true;
	}

	// 死亡アニメのみ更新
	int deadIdx = GetGraphIndex(BossState::Dead);
	animations_[deadIdx]->Update();

	if (animations_[deadIdx]->IsAnimFinished())
		isDeadAnimFinished_ = true;
}

void Boss::OnHit(int damage)
{
	Application::GetInstance().GetSEManager().PlaySE(SE::Hit);
	if (currentState_ == BossState::Dead) return;

	hp_ -= damage;
	if (hp_ <= 0)
	{
		currentState_ = BossState::Dead;
		stateTimer_ = 0;
		vel_ = { 0.0f,0.0f };
		isPlayingDeathEffect_ = false; //死亡演出開始用フラグ
		return;
	}

	StartHitInvincible();
	pCamera_->Shake(kCameraDuration, kCameraMagnitude);
	ChangeState(BossState::Hurt);
}

void Boss::StartHitInvincible()
{
	isHitInvincible_ = true;
	hitInvincibleTimer_ = kHitInvincibleTime;
}
