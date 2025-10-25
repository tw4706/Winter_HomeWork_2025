#include "Player.h"
#include"Dxlib.h"
#include"Game.h"
#include "Input.h"
#include"Geometory.h"

namespace
{
	constexpr float kPlayerPosX = 300.0f;
	constexpr float kPlayerPosY = 300.0f;
	constexpr float kSpeed = 2.0f;
	constexpr float kRadius = 16.0f;
}

Player::Player():
	pos_{},
	playerHandle_(-1)
{
}

Player::~Player()
{
}

void Player::Init()
{
	pos_.x = kPlayerPosX;
	pos_.y = kPlayerPosY;
	playerHandle_ = LoadGraph("data/Player/Player.png");
}

void Player::Update(Input&input)
{
	if (input.IsPressed("left"))
	{
		pos_.x -= kSpeed;
	}
	if (input.IsPressed("right"))
	{
		pos_.x += kSpeed;
	}
	//êßå¿
	if (pos_.x < kRadius)
	{
		pos_.x = kRadius;
	}
	if (pos_.x >= Game::kScreenWidth + kRadius)
	{
		pos_.x = Game::kScreenWidth + kRadius;
	}
	if (pos_.y <= kRadius)
	{
		pos_.y = kRadius;
	}
	if (pos_.y >= Game::kScreenHeight - kRadius)
	{
		pos_.y = Game::kScreenHeight- kRadius;
	}
}

void Player::Draw()
{
	DrawCircle(pos_.x, pos_.y, 16, GetColor(255, 255, 255), true);
}
