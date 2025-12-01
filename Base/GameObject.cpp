#include"GameObject.h"

namespace
{
	constexpr float kGravity = 0.3f;  // 重力
	constexpr float kCharaSize = 32.0f;//キャラクターサイズ
}

GameObject::GameObject():
	pos_{},
	vel_{}
{
}

GameObject::GameObject(Vector2 pos,Vector2 vel):
	pos_(pos),
	vel_(vel),
	isTurn_(false),
	isGround_(false),
	drawOffset_{}
{

}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	Gravity();
}

void GameObject::Draw()
{
#ifdef _DEBUG
	//当たり判定を表示
	colRect_.Draw(0x0000ff, false);
#endif

}

void GameObject::Gravity() 
{
	vel_.y += kGravity;
}

void GameObject::CheckHitMap(Rect& chipRect)
{
	pos_.x += vel_.x;
	colRect_.SetCenter(pos_.x, pos_.y, kCharaSize - 1, kCharaSize - 1);

	if (pBg_->IsCollision(colRect_, chipRect))
	{
		if (vel_.x > 0.0f)
		{
			pos_.x = chipRect.GetLeft() - kCharaSize * 0.5f;
		}
		else if(vel_.x < 0.0f)
		{
			pos_.x = chipRect.GetRight() - kCharaSize * 0.5f;
		}
		vel_.x = 0.0f;
	}

	pos_.y = vel_.y;
	colRect_.SetCenter(pos_.x, pos_.y, kCharaSize - 1, kCharaSize - 1);
	if (pBg_->IsCollision(colRect_, chipRect))
	{
		if (vel_.y > 0.0f)
		{
			pos_.y = chipRect.GetTop() - kCharaSize * 0.5f;
			vel_.y = 0.0f;
			isGround_ = true;
		}
		else if (vel_.y > 0.0f)
		{
			pos_.y = chipRect.GetBottom() + kCharaSize * 0.5f;
			vel_.y *= -1.0f;
		}
	}
}
