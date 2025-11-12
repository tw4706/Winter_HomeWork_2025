#include "Bullet.h"
#include "Input.h"
#include "Rect.h"
#include "Enemy.h"
#include "GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>
#include<cmath>

namespace
{
	constexpr float kScale = 1.5f;
}

Bullet::Bullet(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel),
	pos_(pos),
	vel_(vel),
	isAlive_(true),
	bulletH_(-1)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	bulletH_ = LoadGraph("data/Bullet/lance.png");
	assert(bulletH_ >= 0);
}

void Bullet::Update()
{
	colRect_.SetCenter(pos_.x, pos_.y, 32, 32);
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	pos_ += vel_;
	colRect_.SetCenter(pos_.x, pos_.y, 32, 32);

	for (auto& enemy : enemies)
	{
		if (colRect_.IsCollision(enemy->GetColRect()))
		{
			isAlive_ = false;//弾を消す
			break;
		}
	}

	// 画面外に出たら消滅
	if (pos_.x < 0 || pos_.x > Game::kScreenWidth ||
		pos_.y < 0 || pos_.y > Game::kScreenHeight)
	{
		isAlive_ = false;
	}
}

void Bullet::Draw()
{
	//角度を向きに応じて変更
	float angle = (vel_.x >= 0) ? DX_PI / 2.0f : DX_PI+ (DX_PI / 2.0f);
	DrawRotaGraph(pos_.x,pos_.y,kScale,angle, bulletH_, false);
#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif
}
