#include "Zombie.h"
#include "Player.h"
#include"GrobalConstants.h"
#include <Dxlib.h>
#include <cassert>
#include<cmath>

// エネミーに関する定数
namespace
{
	//グラフィックのサイズ
	constexpr int kGraphW = 32;
	constexpr int kGraphH = 48;

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
	zombieH_ = LoadGraph("data/Enemy/zombie.png");
	assert(zombieH_ >= 0);
}

void Zombie::Update()
{
	Enemy::Update();
	Move();
}

void Zombie::Draw()
{
	if (isRight_)
	{
		DrawRectRotaGraph3(static_cast<int>(pos_.x),
			static_cast<int>(pos_.y),				//描画位置
			0, 0,									//左上の描画開始位置			
			kGraphW, kGraphH,						//描画する矩形のサイズ
			kGraphW / 2, kGraphH / 2,				//回転の中心
			2, 2,									//縦幅と横幅の拡大率
			0,										//回転角度(ラジアン)
			zombieH_, true);
	}
	else
	{
		DrawRectRotaGraph3(static_cast<int>(pos_.x),
			static_cast<int>(pos_.y),				//描画位置
			0, 0,									//左上の描画開始位置			
			kGraphW, kGraphH,						//描画する矩形のサイズ
			kGraphW / 2, kGraphH / 2,				//回転の中心
			2, 2,									//縦幅と横幅の拡大率
			0,										//回転角度(ラジアン)
			zombieH_, true, true);
	}
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

	DrawFormatString(0, 180, GetColor(255, 255, 255), "PlayerX:%f", pPlayer_->GetPos().x);
	DrawFormatString(0, 140, GetColor(255, 255, 255), "dx:%f", dx);
	DrawFormatString(0, 160, GetColor(255, 255, 255), "distance:%f", distance);


	//距離が一定以下の時はプレイヤーに向かって移動
	if (distance < kDistance)
	{
		if (dx < 0) {
			pos_.x -= kSpeed;  // 左へ移動
			isRight_ = false;
		}
		else if (dx > 0) {
			pos_.x += kSpeed;  // 右へ移動
			isRight_ = true;
		}
	}
}
