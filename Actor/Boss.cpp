#include "Boss.h"
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
	constexpr float kScale = 2.0f;
}

Boss::Boss(Vector2 pos, Vector2 vel):
	Enemy(pos,vel),
	state_(BossState::Idle),
	stateTimer_(0),
	handle_(-1)
{
	colSize_ = 64.0f;
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

	//DrawGraph(drawX, drawY, handle_, true);

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

	pos_.y += sin(stateTimer_ * 0.1f) * 0.5f;

	if (stateTimer_ > 120) 
	{
		ChangeState(BossState::Attack);
	}
}

void Boss::UpdateAttack()
{
	stateTimer_++;
	if (stateTimer_ > 180) 
	{
		ChangeState(BossState::Fly);
	}
}

void Boss::UpdateFly()
{
	vel_.x = (isTurn_ ? -3.0f : 3.0f);

	pos_.x += vel_.x;

	// 壁にぶつかったら反転
	if (pos_.x < 200)  isTurn_ = false;
	if (pos_.x > 1000) isTurn_ = true;

	stateTimer_++;

	if (stateTimer_ > 240) {
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateHurt()
{
	stateTimer_++;

	// ノックバックなど
	pos_.x += (isTurn_ ? 2.0f : -2.0f);

	if (stateTimer_ > 30) {
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateDead()
{
	vel_.y += 0.5f;
	pos_.y += vel_.y;

	// 地面へ落下 → 消滅
	if (pos_.y > 1000) {
		isDead_ = true;
	}
}
