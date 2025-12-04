#include "Zombie.h"
#include "Player.h"
#include"GlobalConstants.h"
#include <Dxlib.h>
#include <cassert>
#include<cmath>

// エネミーに関する定数
namespace
{
	//グラフィックのサイズ
	constexpr int kGraphWidth = 32;
	constexpr int kGraphHeight = 48;

	//敵の見た目のサイズ
	constexpr float kDrawW = kGraphWidth * 2.0f;
	constexpr float kDrawH = kGraphHeight * 2.0f;

	//エネミーの移動速度
	constexpr float kSpeed = 0.5f;

	//拡大率
	constexpr float kScale = 2.0f;

	//プレイヤーとの距離
	const float kDistance = 200.0f;
}

Zombie::Zombie(Vector2 pos,Vector2 vel) :
	Enemy(pos,vel),
	zombieH_(-1)
{
}

Zombie::~Zombie()
{
}

void Zombie::Init()
{
	isTurn_ = true;
	zombieH_ = LoadGraph("data/Enemy/zombie_walk.png");
	assert(zombieH_ >= 0);

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y, kDrawW, kDrawH);
}

void Zombie::Update()
{
	if (isDead_)return;
	
	//移動処理
	Move();
	GameObject::Update();

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y-30, kDrawW, kDrawH);
}

void Zombie::Draw()
{
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	DrawRectRotaGraph3(
		drawX, drawY-30,
		0, 0,
		kGraphWidth, kGraphHeight,
		kGraphWidth / 2, kGraphHeight / 2,
		kScale, kScale,
		0.0,
		zombieH_,
		true,
		isTurn_
	);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_,0xff0000, false);
#endif
}

void Zombie::Move()
{
	// プレイヤーがセットされていない場合は何もしない
	if (!pPlayer_) return;

	//プレイヤーとの距離を見て移動する処理を追加
	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = std::abs(dx);

	if (distance < kDistance)
	{
		vel_.x = (dx > 0) ? kSpeed : (dx < 0 ? -kSpeed : 0);
		isTurn_ = dx < 0;
	}
	else
	{
		vel_.x = 0.0f;
	}
	DrawFormatString(0, 80, GetColor(255, 255, 255), "ZombieDx:%f", dx);
	DrawFormatString(0, 100, GetColor(255, 255, 255), "ZombieDistance:%f", distance);
}
