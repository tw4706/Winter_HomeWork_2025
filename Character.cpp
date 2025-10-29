#include "Character.h"

namespace
{
	constexpr float kGravity = 1.5f;  // d—Í
}

Character::Character():
	is_turn(false),
	is_ground(false)
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
	colRect_.SetCenter(pos_.x, pos_.y, width, height);
}

void Character::Draw() 
{
#ifndef _DEBUG
	//“–‚½‚è”»’è‚ğ•\¦
	colRect_.Draw(0x0000ff, false);
#endif
}

void Character::Gravity() {
	vel_.y += kGravity;
}

