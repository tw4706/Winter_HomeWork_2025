#include"Dxlib.h"
#include "Player.h"
#include "Input.h"
#include<cassert>
#include "Character.h"

namespace {
	constexpr int kWidth = 192;//幅
	constexpr int kHeight = 192;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 4.0f;//速度
	constexpr float kJumpPower = 25.0f;//加速度
	//constexpr int kIdleNum=
}


Player::Player() :
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

void Player::Update()
{
}

void Player::Update(Input& input)
{
	Character::Update();
	Move(input);
	if (input.IsPressed("jump"))
	{
		Jump();
	}
	pos_ += vel_;
}

void Player::Draw()
{
	if (is_turn)
	{
		DrawRectGraph(pos_.x, pos_.y, 0, 0, kWidth, kHeight, playerH_, false);
	}
	else
	{
		DrawRectGraph(pos_.x, pos_.y, 0, 0, kWidth, kHeight, playerH_, false, true);
	}
	//DrawGraph(pos_.x, pos_.y, playerH_, true);

}

void Player::Move(Input&input)
{

	if (input.IsPressed("left")) {
		vel_.x = -kSpeed;
		is_turn = false;
	}
	else if (input.IsPressed("right")) {
		vel_.x = kSpeed;
		is_turn = true;
	}
	else {
		vel_.x = 0.0f;
	}

}

void Player::Jump()
{
	//ジャンプ中は処理しない
	if (!is_ground)return;

	vel_.y = -kJumpPower;
	is_ground = false;

}
