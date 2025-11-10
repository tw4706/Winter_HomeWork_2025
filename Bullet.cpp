#include "Bullet.h"
#include "Input.h"
#include "Rect.h"
#include "GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>

Bullet::Bullet(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel),
	pos_(pos),
	vel_(vel),
	isDead_(false),
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

void Bullet::Update(Input&input)
{
	pos_ += vel_;

	// âÊñ äOÇ…èoÇΩÇÁè¡ñ≈
	if (pos_.x < 0 || pos_.x > Game::kScreenWidth|| 
		pos_.y < 0 || pos_.y > Game::kScreenHeight) 
	{
		isDead_ = true;
	}

}

void Bullet::Draw()
{
	DrawGraph(pos_.x,pos_.y, bulletH_, false);
#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif
}
