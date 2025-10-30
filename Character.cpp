#include "Character.h"

namespace
{
	constexpr float kGravity = 0.3f;  // 重力
	constexpr float kCharaSize = 32.0f;//キャラクターサイズ
}

Character::Character() :
	width(0.0f),
	height(0.0f),
	isTurn_(false),
	isGround_(false)
{
}

Character::~Character()
{
}

void Character::Init()
{

}

void Character::Update()
{
	Gravity();
	colRect_.SetCenter(pos_.x, pos_.y, kCharaSize, kCharaSize);
}

void Character::Draw()
{
#ifdef _DEBUG
	//当たり判定を表示
	colRect_.Draw(0x0000ff, false);
#endif

}

void Character::Gravity() {
	vel_.y += kGravity;
}

