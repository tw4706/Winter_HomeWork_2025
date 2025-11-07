#include "Rect.h"
#include <Dxlib.h>
#include<cassert>

Rect::Rect():
	left_(0.0f),
	top_(0.0f),
	right_(0.0f),
	bottom_(0.0f)
{
}

Rect::~Rect()
{
}

void Rect::Draw(unsigned int color, bool isFill)
{
	DrawBox(static_cast<int>(left_), static_cast<int>(top_), static_cast<int>(right_), static_cast<int>(bottom_),color,isFill);
}

void Rect::SetLT(float left, float top, float width, float height)
{
	left_ = left;
	top_ = top;
	right_ = left + width;
	bottom_ = top + height;
}

void Rect::SetCenter(float x, float y, float width, float height)
{
	left_ = x - width / 2;
	top_ = y - height / 2;
	right_ = x + width / 2;
	bottom_ = y + height / 2;
}

float Rect::GetWidth() const
{
	assert(right_ >= left_);//左右の座標の入れ替わりチェック
	return right_ - left_;
}

float Rect::GetHeight() const
{
	assert(bottom_ >= top_);//上下の座標の入れ替わりチェック
	return bottom_ - top_;
}

Vector2 Rect::GetCenter() const
{
	float x = (left_ + right_) / 2;
	float y = (top_ + bottom_) / 2;
	return Vector2{ x,y };
}

bool Rect::IsCollision(const Rect& rect)
{
	//絶対に当たらないパターンを洗い出していく
	if (left_ > rect.right_)return false;
	if (top_ > rect.bottom_)return false;
	if (right_ > rect.left_)return false;
	if (bottom_ > rect.top_)return false;

	//これ以外のパターンは当たっている
	return true;
}
