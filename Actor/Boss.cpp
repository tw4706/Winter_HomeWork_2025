#include "Boss.h"
#include<Dxlib.h>

Boss::Boss(Vector2 pos, Vector2 vel):
	Enemy(pos,vel),
	state_(BossState::Idle),
	stateTimer_(0)
{
}

Boss::~Boss()
{
}

void Boss::Init()
{
	state_ = BossState::Idle;
	stateTimer_ = 0;
}

void Boss::Update()
{
	if (isDead_)return;
	UpdateState();
	Enemy::Update();
}

void Boss::Draw()
{
	if (isDead_)return;
	DrawBox(pos_.x - 32, pos_.y - 48,
		pos_.x + 32, pos_.y + 48, GetColor(255, 0, 0), FALSE);
}

void Boss::Move()
{

}

void Boss::Attack()
{
}

void Boss::UpdateState()
{
}

void Boss::ChangeState()
{
}
