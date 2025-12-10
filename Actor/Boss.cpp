#include "Boss.h"
#include "Player.h"
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

	constexpr int kGraphW = 32;
	constexpr int kGraphH = 32;
	constexpr int kGraphHalfW = 32/2;
	constexpr int kGraphHalfH = 32/2;
	constexpr float kScale = 4.0f;

	constexpr float kKnockBackPos = 2.0f;
	constexpr float kComeBackPos = 0.05f;
	constexpr float kSpeed = 3.0f;

	constexpr float kGravity = 0.5f;
	constexpr float kDistance = 400.0f;
	constexpr float kGround = 1764.0f;
}

Boss::Boss(Vector2 pos, Vector2 vel, std::shared_ptr<Player>player, BulletManager*bm):
	Enemy(pos,vel),
	pPlayer_(player),
	pBm_(bm),
	state_(BossState::Idle),
	stateTimer_(0),
	handle_(-1),
	shotTimer_(0.0f),
	shotInterval_(5.0f),
	hp_(10),
	backPos_(pos)
{
	colSize_ = 64.0f;
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

	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
	}
	state_ = BossState::Idle;
	stateTimer_ = 0;
	handle_ = graphHandles_[kIdleGraph];
}

void Boss::Update()
{
	if (isDead_)return;
	Enemy::Update();
	colRect_.SetCenter(pos_.x,pos_.y,colSize_,colSize_);

	switch (state_)
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

	switch (state_)
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


	DrawRectRotaGraph3(
		drawX, drawY,
		0, 0,
		kGraphW, kGraphH,
		kGraphHalfW, kGraphHalfH,
		kScale, kScale,
		0.0,
		handle_, true);
#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Boss::ChangeState(BossState nextState)
{
	state_ = nextState;
	stateTimer_ = 0;
	switch (nextState)
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

void Boss::UpdateIdle()
{
	stateTimer_++;

	//ノックバックしても元の位置に戻る
	pos_.x += (backPos_.x - pos_.x) * kComeBackPos;
	pos_.y += (backPos_.y - pos_.y) * kComeBackPos;

	//ゆっくり上下移動
	pos_.y += sin(stateTimer_ * 0.1f) * 0.5f;

	if (stateTimer_ > 120)
	{
		ChangeState(BossState::Attack);
	}
}

void Boss::UpdateAttack()
{
	shotTimer_ += 1.0f / 60.0f;
	float distance = std::abs(pos_.x - pPlayer_->GetPos().x);
	if (distance < kDistance)
	{
		if (shotTimer_ >= shotInterval_) {
			shotTimer_ = 0.0f;

			//弾を発射
			Vector2 bulletPos = pos_;
			Vector2 bulletVel = { -kSpeed, 0.0f };

			pBm_->AddEnemyBullet(bulletPos, bulletVel);
		}
	}
}

void Boss::UpdateFly()
{
	stateTimer_++;

	//左右移動
	vel_.x = (isTurn_ ? -kSpeed : kSpeed);
	pos_.x += vel_.x;

	//壁で反転
	if (pos_.x < 200)
	{
		isTurn_ = false;
	}
	else if (pos_.x > 1000)
	{
		isTurn_ = true;
	}

	if (stateTimer_ > 150)
	{
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateHurt()
{
	stateTimer_++;

	pos_.x += (isTurn_ ? -kKnockBackPos : kKnockBackPos);

	pos_.y += sin(stateTimer_ * 0.3f) * 0.3f;

	//タイマーを進めてIdle状態に戻る
	if (stateTimer_ > 30)
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
	if (state_ == BossState::Dead) return;

	hp_ -= damage;

	if (hp_ <= 0)
	{
		ChangeState(BossState::Dead);
		return;
	}

	ChangeState(BossState::Hurt);
}
