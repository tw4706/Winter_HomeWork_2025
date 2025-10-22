#include "Player.h"
#include"Dxlib.h"
#include"Game.h"
#include "Input.h"

namespace
{
	constexpr float kPlayerPosX = 300.0f;
	constexpr float kPlayerPosY = 300.0f;
	constexpr float kSpeed = 10.0f;
	constexpr float kRadius = 16.0f;
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

	if()
	if (input.IsPressed("left"))
	{
		x_ -= kSpeed;
	}
	if (input.IsPressed("right"))
	{
		x_ += kSpeed;
	}
	//êßå¿
	if (x_ < kRadius)
	{
		x_ = kRadius;
	}
	if (x_ >= Game::kScreenWidth + kRadius)
	{
		x_ = Game::kScreenWidth + kRadius;
	}
	if (y_ <= kRadius)
	{
		y_ = kRadius;
	}
	if (y_ >= Game::kScreenHeight - kRadius)
	{
		y_ = Game::kScreenHeight- kRadius;
	}
}

void Player::Draw()
{
	DrawCircle(x_, y_,kRadius, GetColor(255, 255, 255), true);
}
