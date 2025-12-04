#include "Dog.h"
#include "Player.h"
#include "Enemy.h"
#include<Dxlib.h>
#include<cmath>
#include<cassert>

namespace
{
	//画像の切り取りサイズ
	constexpr int kGraphSize = 48;
	constexpr int kGraphHalfSize = kGraphSize -16;

	//当たり判定のサイズ
	constexpr  int kRectWidth = 32;
	constexpr  int kRectHeight = 32;

	constexpr float kDrawW = kGraphSize * 2.0f;
	constexpr float kDrawH = kGraphSize * 2.0f;

	//ジャンプの高さ
	constexpr float kJumpPower = 15.0f;
	//移動速度
	constexpr float kSpeed = 4.0f;
	//ジャンプ間隔
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
	//画像のロード
	dogH_ = LoadGraph("data/Enemy/dog.png");
	assert(dogH_ >= 0);
}

void Dog::Update()
{
	if (isDead_)return;

	//移動処理
	Move();
	GameObject::Update();

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y-5, kGraphSize, kGraphSize);

	//デバッグ表示
	DrawFormatString(0, 150, 0xffffff, "Dog PosX:%f", pos_.x);
	DrawFormatString(0, 170, 0xffffff, "Dog VelX:%f", vel_.x);
	DrawFormatString(0, 190, 0xffffff, "Dog Dist:%f", std::abs(pPlayer_->GetPos().x - pos_.x));
}

void Dog::Draw()
{
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	DrawRectRotaGraph3(drawX, drawY-15,
		0, 0,
		kGraphSize, kGraphSize,
		kGraphHalfSize, kGraphHalfSize,
		2.0, 2.0,
		0.0,
		dogH_, true);
#ifdef _DEBUG
	//当たり判定の描画
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Dog::Move()
{
	timer_++;
	if (!pPlayer_) return;

	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = std::abs(dx);

	if (distance < kDistance)
	{
		if (isGround_ && timer_ > kJumpInterval)
		{
			vel_.y = -kJumpPower;
			vel_.x = (dx > 0) ? kSpeed : -kSpeed;
			isGround_ = false;
			timer_ = 0.0f;
		}
		else if (isGround_)
		{
			vel_.x = 0.0f;
		}
	}
	else
	{
		vel_.x = 0.0f;
	}
}