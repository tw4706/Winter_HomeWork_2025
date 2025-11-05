#include<Dxlib.h>
#include<cassert>
#include "Player.h"
#include "Input.h"
#include"Animation.h"
#include"GrobalConstants.h"

// プレイヤーに関する定数
namespace
{
	constexpr int kGraphW = 128;//幅
	constexpr int kGraphH = 128;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 3.0f;//速度
	constexpr float kHalfSpeed = 1.5f;//ジャンプ時の横移動速度
	constexpr float kJumpPower = 8.0f;//ジャンプの高さ
	constexpr float kDoubleJumpPower = 6.0f;//ダブルジャンプの高さ
	constexpr float kGround = 450.0f; // 地面位置
}

Player::Player(Vector2 pos, Vector2 vel):
	GameObject(pos,Vector2()),
	playerH_(-1),
	isJumping_(false),
	canDoubleJumping_(false)
{
}

Player::~Player()
{
}

void Player::Init()
{
	playerH_ = LoadGraph("data/Player/Idle.png");
	assert(playerH_ >= 0);
}

void Player::Update()
{
}

void Player::Update(Input& input)
{
	GameObject::Update();
	Move(input);
	if (input.IsTriggered("jump"))
	{
		Jump(input);
	}
	pos_ += vel_;

	DrawFormatString(0, 0, GetColor(255, 255, 255), "PlayerX:%f", pos_.x);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "VelX:%f", vel_.x);

	//地面の接地判定
	if (pos_.y >= kGround)
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
		isGround_ = true;
		isJumping_ = false;
		canDoubleJumping_ = false;
	}
}

void Player::Draw()
{
	if (isTurn_)
	{
		DrawRectRotaGraph3(pos_.x, pos_.y,			//描画位置
			0, 0,									//左上の描画開始位置			
			kGraphW, kGraphH,						//描画する矩形のサイズ
			kGraphW / 2, kGraphH / 2 + 32,				//回転の中心
			1.5, 1.5,									//縦幅と横幅の拡大率
			0,										//回転角度(ラジアン)
			playerH_, true);
	}
	else
	{
		DrawRectRotaGraph3(pos_.x, pos_.y,
			0, 0,
			kGraphW, kGraphH,
			kGraphW / 2, kGraphH / 2+32,
			1.5, 1.5,
			0,
			playerH_, true, true);
	}
#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif
}

void Player::Move(Input& input)
{
	if (isGround_||canDoubleJumping_)
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
			vel_.x = -kHalfSpeed;
			isTurn_ = false;
		}
		else if (input.IsPressed("right"))
		{
			vel_.x = kHalfSpeed;
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
	// 通常ジャンプ
	if (isGround_) 
	{
		vel_.y = -kJumpPower;
		isGround_ = false;
		canDoubleJumping_ = true;
		return;
	}

	// 二段ジャンプ
	if (canDoubleJumping_) 
	{
		vel_.y = -kDoubleJumpPower;
		canDoubleJumping_ = false;
	}
}

Vector2 Player::GetPos() const
{
	return pos_;
}
