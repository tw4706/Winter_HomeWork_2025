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

	//地面位置
	constexpr float kGround = 500.0f;
	//弾の存在できる数
	constexpr int kBulletNum = 3;

	//ダメージを受けたときの無敵時間
	constexpr int kDamageDuration = 60;

	constexpr float kGravity = 1.0f;
}

Player::Player(Vector2 pos, Vector2 vel) :
	GameObject(pos, Vector2()),
	isJumping_(false),
	isDoubleJumping_(false),
	isDamaged_(false),
	damageTimer_(0),
	shotTimer_(0),
	state_(PlayerState::Idle),
	currentBulletType_(PlayerBulletType::Knife)
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

void Player::Update()
{
}

void Player::Update(Input& input, BulletManager& bm)
{
	Move(input);
	// ジャンプ処理
	if (input.IsTriggered("jump"))
	{
		Jump(input);
	}
	GameObject::Update();

	if (!isGround_)
	{
		GameObject::Gravity();
	}

	pos_ += vel_;

	colRect_.SetCenter(pos_.x, pos_.y, kGraphWidth, kGraphHeight);

	Rect chipRect;
	if (pBg_->IsCollision(colRect_, chipRect))
	{
		CollisionManager::ResolveCollision(colRect_, pos_, vel_, chipRect);
		if (vel_.y == 0)
		{
			isGround_ = true;
			isJumping_ = false;
			isDoubleJumping_ = false;
		}
		else
		{
			isGround_ = false;
		}
	}
	else
	{
		isGround_ = false;
	}


	//地面の接地判定
	//if (pos_.y >= kGround)
	//{
	//	pos_.y = kGround;//地面の位置に固定
	//	vel_.y = 0.0f;//速度を0に
	//	isGround_ = true;
	//	isJumping_ = false;
	//	isDoubleJumping_ = false;
	//}

#ifdef _DEBUG
	if (input.IsTriggered("changeWeapon"))
	{
		printfDx("武器変えた!\n");
		int weaponType = (static_cast<int>(currentBulletType_) + 1) % 2;
		currentBulletType_ = static_cast<PlayerBulletType>(weaponType);
	}

#endif

	//弾の発射間隔がある場合はカウントダウン
	if (shotTimer_ > 0)shotTimer_--;

	//弾の発射・更新
	if (input.IsTriggered("shot") && shotTimer_ <= 0)
	{
		const auto& config = kBulletConfigs[static_cast<int>(currentBulletType_)];

		//銃口オフセット
		constexpr float gunOffsetX = 40.0f;
		constexpr float gunOffsetY = 20.0f;

		// 描画基準座標（プレイヤーの中心）
		float drawX = pos_.x + drawOffset_.x;
		float drawY = pos_.y + drawOffset_.y;

		// 弾の発射位置（中心基準に揃える）
		float spawnX = drawX + (isTurn_ ? gunOffsetX : -gunOffsetX);
		float spawnY = drawY + gunOffsetY;

		Vector2 spawnPos = { spawnX, spawnY };

		// 弾の速度（向きに応じて）
		Vector2 bulletVel = isTurn_ ? Vector2{ config.speed, 0.0f } : Vector2{ -config.speed, 0.0f };

		auto bullet = std::make_shared<Bullet>(spawnPos, bulletVel, currentBulletType_);

		//弾の初期化
		bullet->Init();
		bullet->SetBg(pBg_);

		//弾の追加
		bm.Init(bullet);

		//発射間隔のリセット
		shotTimer_ = config.shotInterval;
	}
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
	DrawCircle(static_cast<int>(pos_.x + drawOffset_.x),
		static_cast<int>(pos_.y + drawOffset_.y),
		5, GetColor(0, 255, 0), true); // 緑丸



#ifdef _DEBUG
	//デバッグ用
	DrawFormatString(0, 0, GetColor(255, 255, 255), "PlayerX:%f", pos_.x);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "VelX:%f", vel_.x);
#endif
}

void Player::Draw()
{
	float drawX = pos_.x + drawOffset_.x;
	float drawY = pos_.y + drawOffset_.y;
	/*pPlayerAnim_->Draw(idleH_, pos_.x, pos_.y, 1.0f, 0.0f);*/
	if (isTurn_)
	{
		DrawRectRotaGraph3(drawX, drawY,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			1.5, 1.5,
			0.0,
			graphHandles_[kIdleGraph], true, false, false);
	}
	else
	{
		DrawRectRotaGraph3(drawX, drawY,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			1.5, 1.5,
			0.0,
			graphHandles_[kIdleGraph], true, true, false);
	}

#ifdef _DEBUG
	//当たり判定の矩形の色を変える
	colRect_.DrawAndCamera(drawOffset_, isDamaged_ ? 0x0000ff : 0xff0000, false);

#endif


#ifdef _DEBUG
	// プレイヤーの描画範囲を矩形で表示
	int boxLeft = static_cast<int>(drawX - (kGraphWidth / 2) * 1.5f);
	int boxTop = static_cast<int>(drawY - (kGraphHeight / 2) * 1.5f);
	int boxRight = static_cast<int>(drawX + (kGraphWidth / 2) * 1.5f);
	int boxBottom = static_cast<int>(drawY + (kGraphHeight / 2) * 1.5f);

	DrawBox(boxLeft, boxTop, boxRight, boxBottom, GetColor(0, 255, 255), false);
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
	// 通常ジャンプ
	if (isGround_)
	{
		vel_.y = -kJumpPower;
		isGround_ = false;
		isDoubleJumping_ = true;
		return;
	}

	// 二段ジャンプ
	if (isDoubleJumping_)
	{
		vel_.y = -kDoubleJumpPower;
		isDoubleJumping_ = false;
	}
}

//ダメージを受けたときの処理
void Player::OnDamage()
{
	isDamaged_ = true;
	damageTimer_ = kDamageDuration;
}