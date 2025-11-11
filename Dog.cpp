#include "Dog.h"
#include<Dxlib.h>
#include<cmath>

namespace
{
	constexpr float kJumpPower = 15.0f;
	constexpr float kSpeed = 4.0f;
	constexpr float kGround = 450.0f;
	constexpr int kJumpInterval = 120;
	//プレイヤーとの距離
	const float kDistance = 300.0f;
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

	//地面の接地判定
	if (pos_.y >= kGround)
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
		isGround_ = true;
	}

	DrawFormatString(0, 130, 0xffffff, "Dog Timer:%f", timer_);
	DrawFormatString(0, 150, 0xffffff, "Dog PosX:%f", pos_.x);
	DrawFormatString(0, 170, 0xffffff, "Dog VelX:%f", vel_.x);
	DrawFormatString(0, 190, 0xffffff, "Dog Dist:%f", std::abs(pPlayer_->GetPos().x - pos_.x));
}

void Dog::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + 32, pos_.y + 32, 0xffaaaa, false);

}

void Dog::Attack()
{

}

void Dog::Move()
{
	timer_++;
	// プレイヤーがセットされていない場合は何もしない
	if (!pPlayer_) return;

	//プレイヤーとの距離を見て移動する処理を追加
	float playerX = pPlayer_->GetPos().x;
	float enemyX = pos_.x;
	float dx = playerX - enemyX;
	float distance = std::abs(dx);
	//動く向きを決める
	int moveDirection = 0;
	//距離が一定以下の時はプレイヤーに向かって移動
	if (distance < kDistance)
	{
		if (pos_.y >= kGround && timer_ > kJumpInterval)
		{
			vel_.y = -kJumpPower;
			timer_ = 0.0f;


			if (dx > 0)
			{
				vel_.x = kSpeed; //プレイヤーが右 → 右にジャンプ
			}
			else if (dx < 0)
			{
				vel_.x = -kSpeed; //プレイヤーが左 → 左にジャンプ
			}
			else
			{
				vel_.x = 0.0f; //真上にジャンプ
			}
		}
		else if (pos_.y >= kGround)
		{
			//地面にいるがジャンプ条件を満たしていない → 停止
			vel_.x = 0.0f;
		}
		//空中では vel_.x を変更しない（ジャンプ時に決めた方向を維持）
	}
	else
	{
		//プレイヤーが遠いときは停止
		vel_.x = 0.0f;
	}
}




