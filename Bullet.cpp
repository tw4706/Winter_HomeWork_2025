#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	constexpr float kScale = 1.5f;
	constexpr int kBulletWidth = 32;
	constexpr int kBulletHeight = 32;
}

Bullet::Bullet(Vector2 pos, Vector2 vel,BulletType type) :
	GameObject(pos, vel),
	pos_(pos),
	vel_(vel),
	isAlive_(true),
	bulletH_(-1),
	type_(type)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	switch (type_)
	{
	case BulletType::Player:
		bulletH_ = LoadGraph("data/Bullet/lance.png");
		break;

	case BulletType::Enemy:
		//bulletH_ = LoadGraph("data/Bullet/enemy_bullet.png");
		break;
	}
	assert(bulletH_ >= 0);

	colRect_.SetCenter(pos_.x, pos_.y, kBulletWidth, kBulletHeight);

}

void Bullet::Update()
{
	pos_ += vel_;
	colRect_.SetCenter(pos_.x, pos_.y, kBulletWidth, kBulletHeight);

	// 画面外から出たら消える
	if(pos_.x<0||pos_.x>Game::kScreenWidth||
		pos_.y<0||pos_.y>Game::kScreenHeight)
	{
		isAlive_ = false;
	}
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	Update();
	if (type_ == BulletType::Player)
	{
		for (auto& enemy : enemies)
		{
			if (enemy && colRect_.IsCollision(enemy->GetColRect()))
			{
				printf("Collision detected!\n");
				OnHit();
				break;
			}
		}
	}
}

void Bullet::Draw()
{
	//生きてなかったら描画しない
	if (!isAlive_)return;
	//角度を向きに応じて変更
	float angle = (vel_.x >= 0) ? DX_PI / 2.0f : DX_PI+ (DX_PI / 2.0f);
	DrawRotaGraph(pos_.x,pos_.y,kScale,angle, bulletH_, false);
#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif
}

void Bullet::OnHit()
{
	//当たったら消える
	isAlive_ = false;
	if (!isAlive_)
	{
		printfDx("当たった！");
	}
}
