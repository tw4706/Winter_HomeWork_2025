#include "Player.h"
#include "Input.h"
#include"Bullet.h"
#include"Bg.h"
#include "BulletManager.h"
#include "CollisionManager.h"
#include"GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>
#include<string>

// プレイヤーに関する定数
namespace
{

	enum Graph
	{
		kIdleGraph,
		kAttackgraph,

		kGraphNum
	};

	const std::string kGraphName[kGraphNum] =
	{
		"data/Player/Idle.png",
		"data/Player/Attack.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kGraphName));

	//プレイヤーの画像サイズ
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;
	constexpr float kScale = 1.5f;

	//移動速度 //通常:3
	constexpr float kSpeed = 5.0f;

	//ジャンプ時の横移動速度
	constexpr float kHalfSpeed = 1.5f;
	//ジャンプの高さ
	constexpr float kJumpPower = 8.0f;
	//ダブルジャンプの高さ
	constexpr float kDoubleJumpPower = 6.0f;

	//落下判定となる座標
	constexpr float kFallLimit = 2100.0f;

	//弾の存在できる数
	constexpr int kBulletNum = 3;
	//弾がプレイヤーから出る位置のオフセット
	constexpr float kGunOffsetX = 40.0f;
	constexpr float kGunOffsetY = 5.0f;

	//ダメージを受けたときの無敵時間
	constexpr int kDamageDuration = 60;
	//重力
	constexpr float kGravity = 1.0f;
}

Player::Player(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel, kGraphWidth, kGraphHeight, 64.0f),
	initializePos_{ pos },
	isJumping_(false),
	isDoubleJumping_(false),
	isDamaged_(false),
	isTouching_(false),
	damageTimer_(0),
	shotTimer_(0),
	state_(PlayerState::Idle),
	currentBulletType_(BulletType::Knife)
{
}

Player::~Player()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void Player::Init()
{
	graphHandles_.resize(kGraphNum);
	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
	}
}

void Player::Update(Input& input, BulletManager& bm)
{
	GameObject::Update();
	colRect_.SetCenter(pos_.x, pos_.y-10, kGraphWidth/2, kGraphHeight-32);
	Move(input);
	// ジャンプ処理
	Jump(input);

	//発射処理
	Shot(input,bm);

	//落下判定
	if (pos_.y > kFallLimit)
	{
		if (onGameOver_)
		{
			onGameOver_();
		}
		return;
	}

#ifdef _DEBUG
	//デバッグでリスポーン
	if (input.IsTriggered("respawn"))
	{
		ReSpawn();
	}
	//武器の切り替え
	if (input.IsTriggered("changeWeapon"))
	{
		printfDx("武器変えた!\n");
		int weaponType = (static_cast<int>(currentBulletType_) + 1) % 2;
		currentBulletType_ = static_cast<BulletType>(weaponType);
	}

#endif
	//無敵時間
	if (damageTimer_ > 0)
	{
		damageTimer_--;
		if (damageTimer_ == 0)
		{
			isDamaged_ = false;
		}
	}

	//プレイヤーの基準点
	DrawCircle(static_cast<int>(pos_.x + cameraOffset_.x),
		static_cast<int>(pos_.y + cameraOffset_.y),
		5, GetColor(0, 255, 0), true);
#ifdef _DEBUG
	//デバッグ用
	DrawFormatString(0, 0, GetColor(255, 255, 255), "PlayerX:%f", pos_.x);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "VelX:%f", vel_.x);
#endif
}

void Player::Draw()
{
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	//描画
	if (isTurn_)
	{
		DrawRectRotaGraph3(drawX, drawY - 60,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			1.5, 1.5,
			0.0,
			graphHandles_[kIdleGraph], true, false, false);
	}
	else
	{
		DrawRectRotaGraph3(drawX, drawY - 60,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			1.5, 1.5,
			0.0,
			graphHandles_[kIdleGraph], true, true, false);
	}

#ifdef _DEBUG
	//当たり判定の矩形の色を変える
	colRect_.DrawAndCamera(cameraOffset_, isDamaged_ ? 0x0000ff : 0xff0000, false);
#endif
}

//移動処理
void Player::Move(Input& input)
{
	bool CanJumpMove = isGround_ || isDoubleJumping_;
	//地面にいるときかつダブルジャンプが可能な時
	if (CanJumpMove)
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
	if (input.IsTriggered("jump")) {
		// 通常ジャンプ
		if (isGround_)
		{
			vel_.y = -kJumpPower;
			isGround_ = false;
			isDoubleJumping_ = true;
			return;
		}

		//二段ジャンプ
		if (isDoubleJumping_)
		{
			vel_.y = -kDoubleJumpPower;
			isDoubleJumping_ = false;
		}
	}
}

void Player::Shot(Input&input,BulletManager&bm)
{
	if (shotTimer_ > 0)
	{
		shotTimer_--;
	}

	if (input.IsTriggered("shot") && shotTimer_ <= 0)
	{
		const auto& config = kBulletConfigs[static_cast<int>(currentBulletType_)];

		// 弾の発射位置（描画offsetなし）
		float spawnX = pos_.x + (isTurn_ ? kGunOffsetX : -kGunOffsetX);
		float spawnY = pos_.y;
		Vector2 spawnPos = { spawnX, spawnY };

		// 弾の速度
		Vector2 bulletVel = isTurn_ ?
			Vector2{ config.speed, 0.0f } : Vector2{ -config.speed, 0.0f };

		// 弾の生成
		auto bullet = std::make_shared<Bullet>(spawnPos, bulletVel, currentBulletType_);
		bullet->Init();
		bullet->SetBg(pBg_);
		bm.Init(bullet);

		shotTimer_ = config.shotInterval;
	}
}

//ダメージを受けたときの処理
void Player::OnDamage()
{
	isDamaged_ = true;
	damageTimer_ = kDamageDuration;
}

//リスポーン処理
void Player::ReSpawn()
{

	if (pos_.y >= kFallLimit)
	{
		pos_ = initializePos_;
		vel_ = {};
		isGround_ = false;
		isJumping_ = false;
		isDoubleJumping_ = false;

		// 必要なら向きや状態もリセット
		isTurn_ = true;
		state_ = PlayerState::Idle;

		// 無敵やショットのクールダウンを初期化したい場合はここで調整
		isDamaged_ = false;
		damageTimer_ = 0;
		shotTimer_ = 0;
	}

}
