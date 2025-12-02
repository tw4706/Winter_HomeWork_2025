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
	zombieH_ = LoadGraph("data/Enemy/zombie_walk.png");
	assert(zombieH_ >= 0);
}

void Zombie::Update()
{
	Enemy::Update();
	Move();

	////当たり判定の更新
	float centerX = pos_.x + drawOffset_.x;
	float centerY = pos_.y + drawOffset_.y;

	colRect_.SetCenter(centerX, centerY, kDrawW, kDrawH);

	////地面の接地判定
	//if (pos_.y >= kGround)
	//{
	//	pos_.y = kGround;
	//	vel_.y = 0.0f;
	//	isGround_ = true;
	//}
	// 当たり判定の更新（描画スケールを考慮）
	////constexpr float scale = 2.0f;
	////float centerX = pos_.x + drawOffset_.x;
	////float centerY = pos_.y + drawOffset_.y;
	////colRect_.SetCenter(centerX, centerY, kGraphWidth * scale * 0.6f, kGraphHeight * scale * 0.6f);
}

void Zombie::Draw()
{

	float drawX = pos_.x + drawOffset_.x;
	float drawY = pos_.y + drawOffset_.y;

	if (isTurn_)
	{
		DrawRectRotaGraph3(drawX, drawY,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			2.0, 2.0,
			0.0,
			zombieH_, true,isTurn_);
	}
	else
	{
		DrawRectRotaGraph3(drawX, drawY,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			2.0, 2.0,
			0.0,
			zombieH_, true,!isTurn_);
	}

#ifdef _DEBUG
	colRect_.DrawAndCamera(drawOffset_,0xff0000, false);
#endif
}

void Zombie::Attack()
{
}

void Zombie::Move()
{
	// プレイヤーがセットされていない場合は何もしない
	if (!pPlayer_) return;

	//プレイヤーとの距離を見て移動する処理を追加
	float playerX = pPlayer_->GetPos().x;
	float enemyX = pos_.x;
	float dx = playerX - enemyX;
	float distance = std::abs(dx);

	DrawFormatString(0, 80, GetColor(255, 255, 255), "ZombieDx:%f", dx);
	DrawFormatString(0, 100, GetColor(255, 255, 255), "ZombieDistance:%f", distance);


	//距離が一定以下の時はプレイヤーに向かって移動
	if (distance < kDistance)
	{
		if (dx < 0) 
		{
			pos_.x -= kSpeed;  // 左へ移動
			isTurn_ = true;
		}
		else if (dx > 0) 
		{
			pos_.x += kSpeed;  // 右へ移動
			isTurn_ = false ;
		}
	}
}
