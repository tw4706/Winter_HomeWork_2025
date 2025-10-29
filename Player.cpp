#include"Dxlib.h"
#include "Player.h"
#include "Input.h"
#include<cassert>
#include "Character.h"

namespace 
{
	constexpr int kWidth = 192;//幅
	constexpr int kHeight = 192;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 4.0f;//速度
	constexpr float kJumpPower = 25.0f;//ジャンプの高さ
	constexpr float kGround = 400.0f; // 地面位置
}


Player::Player() :
	playerH_(-1),
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
	Character::Update();
	Move(input);
	if (!isJumping_) {
		if (input.IsTriggered("jump"))
		{
			Jump();
		}
	}
	pos_ += vel_;

	//地面の接地判定
	if (pos_.y >= kGround) {
		pos_.y = kGround;
		vel_.y = 0.0f;
		isGround_ = true;
	}

}

void Player::Draw()
{
	if (isTurn_)
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
		isTurn_ = false;
	}
	else if (input.IsPressed("right")) {
		vel_.x = kSpeed;
		isTurn_ = true;
	}
	else {
		vel_.x = 0.0f;
	}

}

void Player::Jump()
{
	//ジャンプ中は処理しない
	if (!isGround_)return;

	vel_.y = -kJumpPower;
	isGround_ = false;

}
