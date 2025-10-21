#include "Player.h"
#include"Dxlib.h"
#include"Game.h"
#include "Input.h"

namespace
{
	constexpr float kPlayerPosX = 300.0f;
	constexpr float kPlayerPosY = 300.0f;
}

Player::Player():
	x_(kPlayerPosX),
	y_(kPlayerPosY)
{
}

Player::~Player()
{
}

void Player::Init()
{
}

void Player::Update(Input&input)
{
	if (input.IsPressed("up"))
	{
		y_ -= 10.0f;
	}
	if (input.IsPressed("down"))
	{
		y_ += 10.0f;
	}
	if (input.IsPressed("left"))
	{
		x_ -= 10.0f;
	}
	if (input.IsPressed("right"))
	{
		x_ += 10.0f;
	}
}

void Player::Draw()
{
	DrawCircle(x_, y_,16.0f, GetColor(255, 255, 255), true);
}
