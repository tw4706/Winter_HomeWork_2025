#include "Player.h"
#include"Dxlib.h"
#include"Game.h"
#include "Input.h"

namespace
{
	constexpr float kPlayerPosX = 300.0f;
	constexpr float kPlayerPosY = 300.0f;
	constexpr float kSpeed = 2.0f;
	constexpr float kRadius = 16.0f;
}

Player::Player():
	x_(0.0f),
	y_(0.0f),
	playerHandle_(-1)
{
}

Player::~Player()
{
}

void Player::Init()
{
	x_ = 300.0f;
	y_ = 300.0f;
	playerHandle_ = LoadGraph("data/Player/Player.png");
}

void Player::Update(Input&input)
{
	if (input.IsPressed("left"))
	{
		x_ -= kSpeed;
	}
	if (input.IsPressed("right"))
	{
		x_ += kSpeed;
	}
}

void Player::Draw()
{
	DrawCircle(x_, y_, kRadius, GetColor(255, 255, 255), true);
}
