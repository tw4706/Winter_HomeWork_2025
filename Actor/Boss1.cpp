#include "Boss1.h"
#include "Player.h"
#include "BulletManager.h"
#include<Dxlib.h>

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
	static_assert(static_cast<int>(kGraphNum) == _countof(kGraphName));


	//状態ごとの総フレーム数
	constexpr int kIdleFrameCount = 4;
	constexpr int kAttackFrameCount = 8;
	constexpr int kFlyFrameCount = 4;
	constexpr int kHurtFrameCount = 4;
	constexpr int kDeathFrameCount = 7;

	//状態ごとの更新フレームの間隔
	constexpr int kIdleFrameInterval = 6;
	constexpr int kAttackFrameInterval = 6;
	constexpr int kFlyFrameInterval = 6;
	constexpr int kHurtFrameInterval = 6;
	constexpr int kDeathFrameInterval = 6;

	//状態ごとのフレーム数とフレームの間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kAttackFrameCount,
		kFlyFrameCount, kHurtFrameCount, kDeathFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval,
		kAttackFrameInterval, kFlyFrameInterval, kHurtFrameInterval, kDeathFrameInterval };
	
	//ボスの描画・当たり判定関連
	constexpr int kGraphWidth = 81;
	constexpr int kGraphHeight = 71;
	constexpr int kGraphHalfW = kGraphWidth / 2;
	constexpr int kGraphHalfH = kGraphHeight / 2;
	constexpr float kScale = 4.0f;
	constexpr float kColWidth = 120.0f;
	constexpr float kColHeight = 120.0f;

	//近づかれすぎた時の逃げる速度
	constexpr float kComeBackPos = 0.5f;
	constexpr float kDistance = 400.0f;

	//ボスのノックバック距離
	constexpr float kKnockBackPos = 2.0f;
	constexpr float kSpeed = 0.5f;

	constexpr float kFlySpeed = 0.4f;
	constexpr float kEscapeSpeed = 1.8f;
	constexpr float kBulletSpeed = 3.0f;
	constexpr float kShotInterval = 5.0f;
	constexpr int KMaxHp = 30;
	constexpr float kRushSpeed = 4.5f;   //突進する速度
	constexpr int   kRushTime = 18;     //突進するフレーム数
}

Boss1::Boss1(Vector2 pos, Vector2 vel,
	std::shared_ptr<Player> player,
	BulletManager* bm,
	std::shared_ptr<Camera> camera)
	:Boss(pos, vel, player, bm, camera),
	escapeTimer_(0),
	knockbackDir_(0),
	chargeVel_(0.0f)
{
	SetUseGravity(false);
	hp_ = KMaxHp;
}

Boss1::~Boss1()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void Boss1::Init()
{
	SetUseGravity(false);
	Boss::Init();
}

void Boss1::Update()
{
	Boss::Update();

	colRect_.SetCenter(pos_.x, pos_.y, kColWidth, kColHeight);
}

void Boss1::LoadResources()
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
			false, 0);
	}
}

void Boss1::OnHit(int damage)
{
	Boss::OnHit(damage);

	if (currentState_ == BossState::Dead) return;

	knockbackDir_ = (pPlayer_->GetPos().x < pos_.x) ? 1 : -1;
}

void Boss1::UpdateIdle()
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
		ChangeState(BossState::Move);
		return;
	}

	//ゆっくり上下移動
	pos_.y += sin(stateTimer_ * 0.1f) * 0.5f;
}

void Boss1::UpdateAttack()
{
	stateTimer_++;

	if (isCharging_)
	{
		pos_.x += chargeVel_.x;
		pos_.y += chargeVel_.y;

		// 突進時間終了でFlyへ
		if (stateTimer_ > kRushTime)
		{
			isCharging_ = false;
			ChangeState(BossState::Move);
		}
		return;
	}

	vel_.x = 0.0f;

	float targetY = pPlayer_->GetPos().y - 120.0f;
	pos_.y += (targetY - pos_.y) * 0.02f;

	// 一定間隔で弾
	shotTimer_ += 1.0f / 60.0f;
	if (shotTimer_ >= 1.5f)
	{
		shotTimer_ = 0.0f;

		Vector2 bulletVel =
		{
			isTurn_ ? -kBulletSpeed : kBulletSpeed,
			0.0f
		};

		pBm_->AddEnemyBullet(pos_, bulletVel);
	}

	// 短時間でFlyに戻る
	if (stateTimer_ > 120)
	{
		ChangeState(BossState::Move);
	}
}

void Boss1::UpdateMove()
{
	stateTimer_++;

	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = fabsf(dx);
	float speed = kFlySpeed;

	//逃げ維持
	if (escapeTimer_ > 0)
	{
		escapeTimer_--;
		speed = kEscapeSpeed;
	}
	else
	{
		//近すぎたら逃げ開始
		if (distance < 300.0f)
		{
			isTurn_ = (dx > 0); //プレイヤーと逆方向
			escapeTimer_ = 40;
			speed = kEscapeSpeed;
		}
		//遠すぎたら接近する
		else if (distance > 500.0f)
		{
			isTurn_ = (dx < 0);
			speed = kFlySpeed;
		}
	}

	vel_.x = (isTurn_ ? -speed : speed);
	pos_.x += vel_.x;

	//上下の追従は弱め
	float targetY = pPlayer_->GetPos().y - 150.0f;
	pos_.y += (targetY - pos_.y) * 0.02f;

	//羽ばたき
	pos_.y += sin(stateTimer_ * 0.08f) * 1.2f;

	//重なり防止
	if (fabsf(dx) < 30.0f)
	{
		isTurn_ = !isTurn_;
		escapeTimer_ = 40;
	}

	//攻撃へ
	if (stateTimer_ > 60 && rand() % 100 < 2)
	{
		ChangeState(BossState::Attack);
	}
}

void Boss1::UpdateHurt()
{
	stateTimer_++;

	int animIndex = GetGraphIndex(BossState::Hurt);
	animations_[animIndex]->Update();

	if (animations_[animIndex]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

int Boss1::GetGraphIndex(BossState state) const
{
	switch (state)
	{
	case BossState::Idle:
		return kIdleGraph;
	case BossState::Attack:
		return kAttackGraph;
	case BossState::Move:
		return kFlyGraph;
	case BossState::Hurt:
		return kHurtGraph;
	case BossState::Dead:
		return kDeadGraph;
	default:
		return kIdleGraph;
	}
	return kIdleGraph;
}
