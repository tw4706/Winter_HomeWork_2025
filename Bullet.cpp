#include "Bullet.h"
#include "Input.h"
#include "GlobalConstants.h"
#include<Dxlib.h>

Bullet::Bullet(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel),
	pos_(pos),
	vel_(vel),
	isDead_(false)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
}

void Bullet::Update()
{
}

void Bullet::Update(Input&input)
{
	pos_ += vel_;

	// âÊñ äOÇ…èoÇΩÇÁè¡ñ≈
	if (pos_.x < 0 || pos_.x > Game::kScreenWidth|| pos_.y < 0 || pos_.y > Game::kScreenHeight) {
		isDead_ = true;
	}

}

void Bullet::Draw()
{
	DrawCircle(pos_.x, pos_.y, 16, 0xff4444, false);
}
