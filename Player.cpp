#include<Dxlib.h>
#include<cassert>
#include "Player.h"
#include "Input.h"
#include "SceneController.h"

namespace
{
	constexpr int kGraphW = 192;//幅
	constexpr int kGraphH = 192;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 3.0f;//速度
	constexpr float kHalfSpeed = 1.5f;//ジャンプ時の横移動速度
	constexpr float kJumpPower = 10.0f;//ジャンプの高さ
	constexpr float kDoubleJumpPower = 8.0f;//ダブルジャンプの高さ
	constexpr float kGround = 400.0f; // 地面位置
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

Player& Player::GetInstance()
{
	static Player instance({ 0,0 }, { 0,0 });
	return instance;
}

void Player::Init()
{
	playerH_ = LoadGraph("data/Player/Player.png");
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
		DrawRectRotaGraph3(pos_.x, pos_.y,	//描画位置
			0, 0,				//左上の描画開始位置			
			kGraphW, kGraphH,	//描画する矩形のサイズ
			0, 0,				//回転の中心
			1, 1,				//縦幅と横幅の拡大率
			0,					//回転角度(ラジアン)
			playerH_, false);
	}
	else
	{
		DrawRectRotaGraph3(pos_.x, pos_.y,
			0, 0,
			kGraphW, kGraphH,
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
