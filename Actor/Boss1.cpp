#include "Boss1.h"
#include "Player.h"
#include "BulletManager.h"
#include "Application.h"
#include<Dxlib.h>

namespace 
{
	//状態毎のグラフハンドル
	enum Graph
	{
		kIdleGraph,
		kAttackGraph,
		kFlyGraph,
		kHurtGraph,
		kDeadGraph,

		kGraphNum
	};

	//ボス1に対応する画像
	const std::string kNormalGraphName[kGraphNum] =
	{
		"data/Enemy/IDLE.png",
		"data/Enemy/ATTACK.png",
		"data/Enemy/FLYING.png",
		"data/Enemy/HURT.png",
		"data/Enemy/DEATH.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kNormalGraphName));

	//ステージ2のボス1の亜種に対応する画像
	const std::string kVariantGraphName[kGraphNum] =
	{
		"data/Enemy/IDLE_Stage2.png",
		"data/Enemy/ATTACK_Stage2.png",
		"data/Enemy/FLYING_Stage2.png",
		"data/Enemy/HURT_Stage2.png",
		"data/Enemy/DEATH_Stage2.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kVariantGraphName));


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
	constexpr int kDeathFrameInterval = 20;

	//状態ごとのフレーム数とフレームの間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kAttackFrameCount,
		kFlyFrameCount, kHurtFrameCount, kDeathFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval,
		kAttackFrameInterval, kFlyFrameInterval, kHurtFrameInterval, kDeathFrameInterval };

	//========================
	// 当たり判定・描画関連
	//========================
	constexpr int kGraphWidth = 81;
	constexpr int kGraphHeight = 71;
	constexpr int kGraphHalfW = kGraphWidth / 2;
	constexpr int kGraphHalfH = kGraphHeight / 2;
	constexpr float kScale = 4.0f;
	constexpr float kColWidth = 120.0f;
	constexpr float kColHeight = 120.0f;

	//========================
	// ステータス関連
	//========================
	constexpr float kKnockBackPos = 2.0f;	//ボスのノックバック距離
	constexpr float kSpeed = 0.5f;

	constexpr int KNormalBoss1MaxHp = 10;
	constexpr int KVariantBoss1MaxHp = 20;

	//========================
	// ボスの行動関連
	//========================
	constexpr int   kIdleToMoveMinTime = 60;
	constexpr int   kMoveToAttackMinTime = 60;
	constexpr int   kStateChangeRandBase = 100;
	constexpr int   kStateChangeRandRate = 2;

	//========================
	// Idle関連
	//========================
	constexpr float kIdleHoverSpeed = 0.1f;
	constexpr float kIdleHoverPower = 0.8f;
	constexpr float kComeBackRatio = 0.5f;

	//========================
	// Attack関連
	//========================
	constexpr float kBulletSpeed = 3.0f;
	constexpr int kMaxShotCount = 1;
	constexpr float kShotInterval = 5.0f;
	constexpr float kTargetYOffset = 150.0f;
	constexpr float kFollowRate = 0.02f;
	constexpr float kShotCoolTime = 2.0f;
	constexpr float kFrameTime = 1.0f / 60.0f;

	//========================
	// Move関連
	//========================
	constexpr float kFlySpeed = 0.4f;
	constexpr float kComeBackPos = 0.5f;//プレイヤーが近づいた時の逃げる速度
	constexpr float kDistance = 200.0f;
	constexpr float kMoveHoverSpeed = 0.08f;
	constexpr float kMoveHoverPower = 1.0f;
	constexpr float kOverlapAvoidDistance = 30.0f;
	constexpr int   kEscapeTime = 40;
}

Boss1::Boss1(Vector2 pos, Vector2 vel,
	std::shared_ptr<Player> player,
	BulletManager* bm,
	std::shared_ptr<Camera> camera, EffectManager* effectMgr, Boss1Type type)
	:Boss(pos, vel, player, bm, camera,effectMgr),
	type_(type),
	escapeTimer_(0),
	shotCount_(0),
	knockbackDir_(0),
	chargeVel_(0.0f)
{
	SetUseGravity(false);

	if (type_ == Boss1Type::Variant)
	{
		hp_ = KVariantBoss1MaxHp;
	}
	else
	{
		hp_ = KNormalBoss1MaxHp;
	}
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
	shotCount_ = 0;
	SetUseGravity(false);

	Boss::Init();
}

void Boss1::Update()
{
	//Boss基本処理
	Boss::Update();

	if (currentState_ == BossState::Dead)
	{
		UpdateDead();
		return;
	}

	//当たり判定更新
	colRect_.SetCenter(pos_.x, pos_.y, kColWidth, kColHeight);

	switch (currentState_)
	{
	case BossState::Idle: UpdateIdle(); break;
	case BossState::Attack: UpdateAttack(); break;
	case BossState::Move: UpdateMove(); break;
	case BossState::Hurt: UpdateHurt(); break;
	}
}

void Boss1::LoadResources()
{
	graphHandles_.resize(kGraphNum);
	animations_.resize(kGraphNum);

	const std::string* graphNames =
		(type_ == Boss1Type::Variant)
		? kVariantGraphName
		: kNormalGraphName;

	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(graphNames[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			(i != kHurtGraph && i != kDeadGraph),
			0);
	}
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
	if (stateTimer_ > kIdleToMoveMinTime && rand() % kStateChangeRandBase < kStateChangeRandRate)
	{
		ChangeState(BossState::Move);
		return;
	}

	//ゆっくり上下移動
	pos_.y += sin(stateTimer_ * kIdleHoverSpeed) * kIdleHoverPower;
}

void Boss1::UpdateAttack()
{
	if (stateTimer_ == 0)
	{
		shotCount_ = 0;
		shotTimer_ = 0.0f;
	}

	stateTimer_++;

	float targetY = pPlayer_->GetPos().y - kTargetYOffset;
	pos_.y += (targetY - pos_.y) * kFollowRate;

	//一定間隔で弾
	shotTimer_ += kFrameTime;
	if (shotTimer_ >= kShotCoolTime&& shotCount_ < kMaxShotCount)
	{
		shotTimer_ = 0.0f;
		shotCount_++;

		Vector2 bulletVel = { isTurn_ ? -kBulletSpeed : kBulletSpeed,0.0f };

		pBm_->AddBoss1Bullet(pos_, bulletVel);
		Application::GetInstance().GetSEManager().PlaySE(SE::BossBreath);
	}

	if (shotCount_ >= kMaxShotCount)
	{
		ChangeState(BossState::Move);
	}
}

void Boss1::UpdateMove()
{
	if (stateTimer_ == 0)
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::BossMove);
	}

	stateTimer_++;

	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = fabsf(dx);
	float speed = kFlySpeed;

	//上下の追従は弱め
	float targetY = pPlayer_->GetPos().y - kTargetYOffset;
	pos_.y += (targetY - pos_.y) * kFollowRate;

	//羽ばたき
	pos_.y += sin(stateTimer_ * kMoveHoverSpeed) * kMoveHoverPower;

	//重なり防止
	if (fabsf(dx) < kOverlapAvoidDistance)
	{
		isTurn_ = !isTurn_;
		escapeTimer_ = kEscapeTime;
	}

	//攻撃へ
	if (stateTimer_ > kMoveToAttackMinTime && rand() % kStateChangeRandBase < kStateChangeRandRate)
	{
		ChangeState(BossState::Attack);
		Application::GetInstance().GetSEManager().StopSE(SE::BossMove);
	}
}

void Boss1::UpdateHurt()
{
	stateTimer_++;

	int animIndex = GetGraphIndex(BossState::Hurt);
	animations_[animIndex]->Update();

	if (animations_[animIndex]->IsAnimFinished())
	{
		ChangeState(BossState::Attack);
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