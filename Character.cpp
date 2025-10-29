#include "Character.h"

namespace
{
	constexpr float kGravity = 1.5f;  // èdóÕ
}

Character::Character():
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
}

void Character::Draw() {
}

void Character::Gravity() {
	vel_.y += kGravity;
}

