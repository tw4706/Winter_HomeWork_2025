#include "Character.h"

namespace
{
	constexpr float kGravity = 1.5f;  // 重力
	constexpr float kGround = 400.0f; // 地面位置
}

Character::Character():
	jump_frame(0),
	is_turn(false),
	is_ground(false)
{
}

Character::~Character()
{
}

void Character::Init() {

}

void Character::Update() {
	Gravity();
	//地面の接地判定
	if (pos_.y >= kGround) {
		pos_.y = kGround;
		vel_.y = 0.0f;
		is_ground = true;
		jump_frame = 0;
	}
}

void Character::Draw() {
}

void Character::Gravity() {
	vel_.y += kGravity;
}

