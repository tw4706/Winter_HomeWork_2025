#include "Dog.h"
#include<Dxlib.h>

namespace
{
	constexpr float kJumpPower = 15.0f;
	constexpr float kSpeed = 4.0f;
	constexpr float kGround = 450.0f;
	constexpr int kJumpInterval = 120;
}

Dog::Dog(Vector2 pos, Vector2 vel) :
	Enemy(pos, vel),
	dogH_(-1),
	timer_(0.0f)
{
}

Dog::~Dog()
{
}

void Dog::Init()
{
	//dogH_ = LoadGraph();

}

void Dog::Update()
{
	Enemy::Update();
	Move();
	pos_ += vel_;

	GameObject::Gravity();

	//’n–Ê‚ÌÚ’n”»’è
	if (pos_.y >= kGround)
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
		isGround_ = true;
	}
}

void Dog::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + 32, pos_.y + 32, 0xffaaaa, false);
	DrawFormatString(0, 300, 0xffffff, "x:%f,y:%f", pos_.x, pos_.y);
}

void Dog::Attack()
{

}

void Dog::Move()
{
	timer_++;
	if (pos_.y >= kGround&&timer_>kJumpInterval)
	{
		vel_.y = -kJumpPower;
		/*vel_.x = -kSpeed;*/
		timer_ = 0.0f;
	}
}

