#include<Dxlib.h>
#include<cassert>
#include "Player.h"
#include "Input.h"
#include "BulletManager.h"
#include"GlobalConstants.h"

// プレイヤーに関する定数
namespace
{
	constexpr int kGraphW = 128;//幅
	constexpr int kGraphH = 128;//高さ
	constexpr float kRadius = 16.0f;//半径
	constexpr float kSpeed = 3.0f;//速度

	//ジャンプ時の横移動速度
	constexpr float kHalfSpeed = 1.5f;
	//ジャンプの高さ
	constexpr float kJumpPower = 8.0f;
	//ダブルジャンプの高さ
	constexpr float kDoubleJumpPower = 6.0f;

	//地面位置
	constexpr float kGround = 450.0f; 
	//弾の存在できる数
	constexpr int kBulletNum = 3; 
}

Player::Player(Vector2 pos, Vector2 vel) :
	GameObject(pos, Vector2()),
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
	//初期化処理
	playerH_ = LoadGraph("data/Player/Idle.png");
	assert(playerH_ >= 0);
}

void Player::Update()
{
}

void Player::Update(Input& input, BulletManager& bm)
{
	GameObject::Update();
	Move(input);
	// ジャンプ処理
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

	//弾の発射・更新
	if (input.IsTriggered("shot"))
	{
		Vector2 bulletVel_ = isTurn_ ? Vector2{ 10.0f,0.0f } : Vector2{ -10.0f,0.0f };
		auto bullet = std::make_shared<Bullet>(pos_, bulletVel_, BulletType::Player);
		bullet->Init();
		bm.AddBullet(bullet);
	}

#ifdef _DEBUG
	//デバッグ用
	DrawFormatString(0, 0, GetColor(255, 255, 255), "PlayerX:%f", pos_.x);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "VelX:%f", vel_.x);
#endif


}

void Player::Draw()
{
	//プレイヤーの描画
	if (isTurn_)
	{
		DrawRectRotaGraph3(static_cast<int>(pos_.x), 
			static_cast<int>(pos_.y),					//描画位置
			0, 0,										//左上の描画開始位置			
			kGraphW, kGraphH,							//描画する矩形のサイズ
			kGraphW / 2, kGraphH / 2 + 32,				//回転の中心
			1.5, 1.5,									//縦幅と横幅の拡大率
			0,											//回転角度(ラジアン)
			playerH_, true);
	}
	else
	{
		DrawRectRotaGraph3(static_cast<int>(pos_.x),
			static_cast<int>(pos_.y),
			0, 0,
			kGraphW, kGraphH,
			kGraphW / 2, kGraphH / 2 + 32,
			1.5, 1.5,
			0,
			playerH_, true, true);
	}

#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif
}

//移動処理
void Player::Move(Input& input)
{
	//地面にいるときかつダブルジャンプが可能な時
	if (isGround_ || canDoubleJumping_)
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
	else if (!isGround_)//空中移動
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

//ジャンプ処理
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

//ポジションを返す関数
Vector2 Player::GetPos() const
{
	return pos_;
}
