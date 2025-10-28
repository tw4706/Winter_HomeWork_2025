#include"Dxlib.h"
#include "Player.h"
#include "Input.h"
#include<cassert>

namespace {
	constexpr float kRadius = 16.0f;
	constexpr float kSpeed = 4.0f;
	//constexpr int kIdleNum=
}


Player::Player():
	playerH_(-1)
{
}

Player::~Player()
{
}

void Player::Init()
{
	pos_ = { 300.0f,300.0f };
	playerH_ = LoadGraph("data/Player/Player.png");
	assert(playerH_ >= 0);
}

void Player::Update() {

}

void Player::Update(Input& input)
{
	if (input.IsPressed("left"))
	{
		pos_.x -= kSpeed;
	}
	if (input.IsPressed("right"))
	{
		pos_.x += kSpeed;
	}
}

void Player::Draw()
{

	DrawGraph(pos_.x, pos_.y, playerH_, true);
}