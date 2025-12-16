#include "Boss.h"
#include "Player.h"
#include"Camera.h"
#include "BulletManager.h"
#include<Dxlib.h>
#include<cassert>
#include<string>

namespace
{
	enum Graph
	{
		kIdleGraph,
		kAttackGraph,
		kFlyGraph,
		kHurtGraph,
		kDeadGraph,

		kGraphNum
	};

	const std::string kGraphName[kGraphNum] =
	{
		"data/Enemy/IDLE.png",
		"data/Enemy/ATTACK.png",
		"data/Enemy/FLYING.png",
		"data/Enemy/HURT.png",
		"data/Enemy/DEATH.png"
	};

	static_assert(static_cast<int>(kGraphNum)== _countof(kGraphName));

	constexpr int kGraphWidth = 81;
	constexpr int kGraphHeight = 71;
	constexpr int kGraphHalfW = kGraphWidth /2;
	constexpr int kGraphHalfH = kGraphHeight /2;
	constexpr float kScale = 4.0f;
	constexpr float kColScale = 2.0f;
	constexpr float kColSize = 64.0f;

	constexpr float kKnockBackPos = 2.0f;
	constexpr float kComeBackPos = 0.5f;
	constexpr float kSpeed = 0.5f;
	constexpr float kBulletSpeed = 3.0f;
	constexpr float kShotInterval = 5.0f;
	constexpr int KMaxHp = 10;

	constexpr float kGravity = 0.5f;
	constexpr float kDistance = 400.0f;
	constexpr float kGround = 1764.0f;

	constexpr int kCameraDuration = 10;
	constexpr int kCameraMagnitude = 8;

	//状態ごとの総フレーム数
	constexpr int kIdleFrameCount = 4;
	constexpr int kAttackFrameCount = 8;
	constexpr int kFlyFrameCount = 4;
	constexpr int kHurtFrameCount = 4;
	constexpr int kDeathFrameCount = 7;

	constexpr int kIdleFrameInterval = 6;
	constexpr int kAttackFrameInterval = 6;
	constexpr int kFlyFrameInterval = 6;
	constexpr int kHurtFrameInterval = 6;
	constexpr int kDeathFrameInterval = 12;

	//状態ごとのフレーム数とフレーム間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kAttackFrameCount,
		kFlyFrameCount, kHurtFrameCount, kDeathFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval,
		kAttackFrameInterval, kFlyFrameInterval, kHurtFrameInterval, kDeathFrameInterval };
}

Boss::Boss(Vector2 pos, Vector2 vel, std::shared_ptr<Player>player, BulletManager*bm,std::shared_ptr<Camera>camera):
	Enemy(pos,vel),
	pPlayer_(player),
	pBm_(bm),
	pCamera_(camera),
	currentState_(BossState::Idle),
	stateTimer_(0),
	handle_(-1),
	shotTimer_(0.0f),
	shotInterval_(kShotInterval),
	hp_(KMaxHp),
	backPos_(pos),
	hasShot_(false),
	knockbackDir_(0)
{
	colSize_ = kColSize;
	SetUseGravity(false);
}

Boss::~Boss()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void Boss::Init()
{
	graphHandles_.resize(kGraphNum);
	animations_.resize(kGraphNum);

	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			false,0
		);
	}
	currentState_ = BossState::Idle;
	stateTimer_ = 0;
	handle_ = graphHandles_[kIdleGraph];
}

void Boss::Update()
{
	if (isDead_)return;
	Enemy::Update();

	//常にプレイヤーの方向を向く
	isTurn_ = (pPlayer_->GetPos().x < pos_.x);

	colRect_.SetCenter(pos_.x-20, pos_.y, colSize_ * kColScale, colSize_ * kColScale);

	//アニメーションの更新
	animations_[static_cast<int>(currentState_)]->Update();

	switch (currentState_)
	{
	case BossState::Idle:
		UpdateIdle();
		break;
	case BossState::Attack:
		UpdateAttack();
		break;
	case BossState::Fly:
		UpdateFly();
		break;
	case BossState::Hurt:
		UpdateHurt();
		break;
	case BossState::Dead:
		UpdateDead();
		break;
	}
}

void Boss::Draw()
{
	if (isDead_)return;

	switch (currentState_)
	{
	case BossState::Idle:
		handle_ = graphHandles_[kIdleGraph];
		break;
	case BossState::Attack:
		handle_ = graphHandles_[kAttackGraph];
		break;
	case BossState::Fly:
		handle_ = graphHandles_[kFlyGraph];
		break;
	case BossState::Hurt:
		handle_ = graphHandles_[kHurtGraph];
		break;
	case BossState::Dead:
		handle_ = graphHandles_[kDeadGraph];
		break;
	}

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;
	//描画
	int animIndex = static_cast<int>(currentState_);
	int frame = animations_[animIndex]->GetFrameCount();

	//アニメーションの描画
	animations_[animIndex]->Draw(drawX, drawY, !isTurn_);
#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Boss::ChangeState(BossState nextState)
{
	if (currentState_ == BossState::Hurt && nextState == BossState::Idle)
	{
		backPos_ = pos_;
	}

	currentState_ = nextState;
	stateTimer_ = 0;

	animations_[static_cast<int>(nextState)]->Reset();
	
	if (nextState == BossState::Attack)
	{
		hasShot_ = false;
	}
}

void Boss::UpdateIdle()
{
	stateTimer_++;

	//ノックバックしても元の位置に戻る
	pos_.x += (backPos_.x - pos_.x) * kComeBackPos;
	pos_.y += (backPos_.y - pos_.y) * kComeBackPos;

	float distance = std::abs(pos_.x - pPlayer_->GetPos().x);

	//プレイヤーが近いと攻撃アニメーション
	if (distance < kDistance)
	{
		ChangeState(BossState::Attack);
		return;
	}

	//一定時間経過で飛行アニメーション
	if (stateTimer_ > 60 && rand() % 100 < 2)
	{
		ChangeState(BossState::Fly);
		return;
	}

	//ゆっくり上下移動
	pos_.y += sin(stateTimer_ * 0.1f) * 0.5f;
}

void Boss::UpdateAttack()
{
	stateTimer_++;

	int currentFrame = animations_[static_cast<int>(BossState::Attack)]->GetCurrentFrame();

	shotTimer_ += 1.0f / 60.0f;
	float distance = std::abs(pos_.x - pPlayer_->GetPos().x);
	if (distance < kDistance)
	{
		if (!hasShot_ && currentFrame == 4)
		{
			if (shotTimer_ >= shotInterval_)
			{
				shotTimer_ = 0.0f;
				hasShot_ = true;

				//弾を発射
				Vector2 bulletPos = pos_;

				//弾がプレイヤーを狙う
				Vector2 toPlayer = pPlayer_->GetPos() - pos_;

				float len = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
				if (len > 0.0f)
				{
					toPlayer.x /= len;
					toPlayer.y /= len;
				}
				Vector2 bulletVel = { toPlayer.x * kBulletSpeed, toPlayer.y * kBulletSpeed };

				pBm_->AddEnemyBullet(bulletPos, bulletVel);
			}
		}
	}
	if (animations_[static_cast<int>(BossState::Attack)]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateFly()
{
	stateTimer_++;

	// プレイヤーが右なら右へ、左なら左へ
	float playerX = pPlayer_->GetPos().x;
	isTurn_ = (playerX < pos_.x); //プレイヤーが左にいるなら反転

	// 水平方向の移動
	vel_.x = (isTurn_ ? -kSpeed : kSpeed);
	pos_.x += vel_.x;

	// 大きく上下へ蛇行 (振幅40〜60)
	float amplitude = 40.0f;
	float frequency = 0.8f;
	pos_.y += sin(stateTimer_ * frequency) * amplitude * 0.1f;

	// 一定時間で戻る
	if (animations_[static_cast<int>(BossState::Fly)]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateHurt()
{
	stateTimer_++;

	pos_.x += (isTurn_ ? kKnockBackPos : -kKnockBackPos);

	pos_.y += sin(stateTimer_ * 0.3f) * 0.3f;

	//タイマーを進めてIdle状態に戻る
	if (animations_[static_cast<int>(BossState::Hurt)]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateDead()
{
	vel_.y += kGravity; //重力
	pos_.y += vel_.y;

	if (pos_.y > kGround)
	{
		pos_.y = kGround;
		isDead_ = true;
	}
}

void Boss::OnHit(int damage)
{
	if (currentState_ == BossState::Dead) return;

	hp_ -= damage;

	pCamera_->Shake(kCameraDuration, kCameraMagnitude);

	if(pPlayer_->GetPos().x < pos_.x)
	{
		knockbackDir_ = 1; //右方向にノックバック
	}
	else
	{
		knockbackDir_ = -1; //左方向にノックバック
	}

	if (hp_ <= 0)
	{
		ChangeState(BossState::Dead);
		return;
	}

	//すでにダメージを受けているなら上書をしないようにする
	if (currentState_ != BossState::Hurt)
	{
		ChangeState(BossState::Hurt);
	}

}
