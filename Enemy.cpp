#include "Enemy.h"
#include "Player.h"
#include<Dxlib.h>
#include<cmath>

namespace
{
	//エネミーの開始位置
	constexpr int kStartX = 500;
	constexpr int kStartY = 300;
	//エネミーの移動速度
	constexpr int kSpeed = 2;
	//プレイヤーとの距離
	const float kDistance = 200.0f;
}

Enemy::Enemy(Vector2 pos):
	GameObject(pos,Vector2()),
	isRight_(false)
{
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Move();
}

void Enemy::Move()
{
	//プレイヤーとの距離を見て移動する処理を追加
	int playerX = Player::GetInstance().GetPos().x;
	int enemyX = GetPos().x;
	int dx = playerX - enemyX;
	int distance = abs(dx);

	DrawFormatString(0, 20, GetColor(255, 255, 255), "PlayerX: %d", playerX);
	DrawFormatString(0, 40, GetColor(255, 255, 255), "EnemyX: %d", enemyX);

	DrawFormatString(0, 0, GetColor(255, 255, 255), "距離: %d", distance);
	//距離が一定以下ならプレイヤーに向かって移動
	if (distance < kDistance)
	{
		if (playerX < enemyX)
		{
			pos_.x -= kSpeed;
			isRight_ = false;
		}
		else if (playerX > enemyX)
		{
			pos_.x += kSpeed;
			isRight_ = true;
		}
	}

}

bool Enemy::IsDead() const
{
	return false;
}
