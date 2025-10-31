#include<Dxlib.h>
#include<cassert>
#include "Player.h"
#include "Input.h"
#include "Character.h"
#include "SceneController.h"

namespace
{
	constexpr int kWidth = 192;//幅
	constexpr int kHeight = 192;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 3.0f;//速度
	constexpr float kJumpPower = 5.0f;//ジャンプの高さ
	constexpr float kDoubleJumpPower = 10.0f;//ダブルジャンプの高さ
	constexpr float kGround = 400.0f; // 地面位置
}


Player::Player() :
	playerH_(-1),
	isJumping_(false),
	isDoubleJump_(false)
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
	if (input.IsTriggered("jump"))
	{
		Jump(input);
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
		DrawRectRotaGraph3(pos_.x, pos_.y,	//描画位置
			0, 0,				//左上の描画開始位置			
			kWidth, kHeight,	//描画する矩形のサイズ
			0, 0,				//回転の中心
			1, 1,				//縦幅と横幅の拡大率
			0,					//回転角度(ラジアン)
			playerH_, false);
	}
	else
	{
		DrawRectRotaGraph3(pos_.x, pos_.y,
			0, 0,
			kWidth, kHeight,
			0, 0,
			1, 1,
			0,
			playerH_, false, true);
	}
#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif
}

void Player::Move(Input& input)
{
	if (isGround_)
	{
		if (input.IsPressed("left"))
		{
			vel_.x = -kSpeed;
			isTurn_ = false;
		}
		else if (input.IsPressed("right"))
		{
			vel_.x = kSpeed;
			isTurn_ = true;
		}
		else
		{
			vel_.x = 0.0f;
		}
	}
	else if (!isGround_)
	{
		if (input.IsPressed("left"))
		{
			vel_.x = -(kSpeed/2);
			isTurn_ = false;
		}
		else if (input.IsPressed("right"))
		{
			vel_.x = kSpeed / 2;
			isTurn_ = true;
		}
		else
		{
			vel_.x = 0.0f;
		}
	}
}

void Player::Jump(Input& input)
{
	//ジャンプ中は処理しない
	if (!isGround_)return;
	vel_.y = -kJumpPower;
	isGround_ = false;
}
