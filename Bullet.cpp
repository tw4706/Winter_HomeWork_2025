#include "Bullet.h"
#include "Input.h"
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

void Bullet::Update(Input&input)
{
	if (input.IsTriggered("shot"))
	{
		pos_.x += vel_.x;
	}

}

void Bullet::Draw()
{
	DrawCircle(pos_.x, pos_.y, 16, 0xff4444, false);
}
