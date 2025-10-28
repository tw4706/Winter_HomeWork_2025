#include"Dxlib.h"
#include "Player.h"
#include "Input.h"
#include<cassert>

namespace {
	constexpr int kWidth = 192;//幅
	constexpr int kHeight = 192;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 4.0f;//速度
	constexpr float kGravity = 5.0f;//加速度
	constexpr float kJumpPower = 80.0f;//加速度
	constexpr float kGround = 300.0f;//着地位置
	//constexpr int kIdleNum=
}


Player::Player():
	frameCount_(0),
	playerH_(-1),
	isTurn_(false),
	isJumping_(false)
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

void Player::Update()
{

}

void Player::Update(Input& input)
{
	GameObject::Update();
	if (input.IsPressed("left"))
	{
		pos_.x -= kSpeed;
		isTurn_ = false;
	}
	if (input.IsPressed("right"))
	{
		pos_.x += kSpeed;
		isTurn_ = true;
	}
	if (input.IsPressed("jump"))
	{
		Gravity(input);
		Jump(input);
		if (pos_.y >= kGround)
		{
			pos_.y = kGround;

		}
	}
}

void Player::Draw()
{
	GameObject::Draw();
	if (!isTurn_)
	{
		DrawRectGraph(pos_.x, pos_.y, 0, 0, kWidth, kHeight, playerH_, false);
	}
	else
	{
		DrawRectGraph(pos_.x, pos_.y, 0, 0, kWidth, kHeight, playerH_, false,true);
	}
	//DrawGraph(pos_.x, pos_.y, playerH_, true);
	
}

void Player::Gravity(Input&input)
{
	pos_.y += kGravity;
}

void Player::Jump(Input& input)
{
	if (input.IsPressed("jump") && !isJumping_)
	{
		pos_.y -= kJumpPower;
		isJumping_ = true;
	}
}
