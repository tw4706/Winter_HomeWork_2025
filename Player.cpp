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
	x_(kPlayerPosX),
	y_(kPlayerPosY),
	playerHandle_(-1)
{
}

Player::~Player()
{
}

void Player::Init()
{
	playerHandle_ = LoadGraph("data/Player/Player_Idle.png");
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
	DrawGraph(x_, y_, playerHandle_, true);
}
