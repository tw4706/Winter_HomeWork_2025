#include "Character.h"

namespace
{
	constexpr float kGravity = 1.5f;  // 重力
	constexpr float kCharaSize = 192.0f;//キャラクターサイズ
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
	//当たり判定を表示
	colRect_.Draw(0x0000ff, false);
}

void Character::Gravity() {
	vel_.y += kGravity;
}

