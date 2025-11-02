#include "Zombie.h"
#include <Dxlib.h>
#include <cassert>

namespace
{
	constexpr int kGraphW = 32;
	constexpr int kGraphH = 48;
	constexpr int kStartX = 600;
	constexpr int kStartY = 300;
}

Zombie::Zombie(Vector2 pos):
	Enemy(pos),
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
	Enemy::Move();
}

void Zombie::Draw()
{
	if (isRight_)
	{
		DrawRectRotaGraph3(pos_.x, pos_.y,	//描画位置
			0, 0,				//左上の描画開始位置			
			kGraphW, kGraphH,	//描画する矩形のサイズ
			0, 0,				//回転の中心
			2, 2,				//縦幅と横幅の拡大率
			0,					//回転角度(ラジアン)
			zombieH_, false);
	}
	else
	{
		DrawRectRotaGraph3(pos_.x, pos_.y,	//描画位置
			0, 0,				//左上の描画開始位置			
			kGraphW, kGraphH,	//描画する矩形のサイズ
			0, 0,				//回転の中心
			2, 2,				//縦幅と横幅の拡大率
			0,					//回転角度(ラジアン)
			zombieH_, false,true);
	}
}

void Zombie::Attack()
{
}

void Zombie::Move()
{
}
