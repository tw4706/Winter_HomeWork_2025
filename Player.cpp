#include<Dxlib.h>
#include<cassert>
#include "Player.h"
#include "Input.h"
#include"Bullet.h"
#include"Bg.h"
#include "BulletManager.h"
#include"GlobalConstants.h"

// プレイヤーに関する定数
namespace
{
	//プレイヤーの画像サイズ
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;
	constexpr float scale = 1.5f;

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
}

Player::Player(Vector2 pos, Vector2 vel) :
	GameObject(pos, Vector2()),
	idleH_(-1),
	attackH_(-1),
	isJumping_(false),
	canDoubleJumping_(false),
	isDamaged_(false),
	damageTimer_(0),
	shotTimer_(0),
	state_(PlayerState::Idle),
	currentBulletType_(PlayerBulletType::Knife)
{
	pPlayerAnim_ = std::make_shared<Animation>();
}

Player::~Player()
{
	DeleteGraph(idleH_);
	DeleteGraph(attackH_);
}

void Player::Init()
{
	//初期化処理
	idleH_ = LoadGraph("data/Player/Idle.png");
	assert(idleH_ > 0);
	if (idleH_ == -1)
	{
		printfDx("失敗!\n");
	}
	attackH_ = LoadGraph("data/Player/Attack.png");
	assert(attackH_ > 0);
	pPlayerAnim_->Init(kGraphWidth, kGraphHeight, 8, 20);
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
	pos_ += vel_;

	GameObject::Update();
	pPlayerAnim_->Update();
	colRect_.SetCenter(pos_.x, pos_.y + 30, kGraphWidth / 2, kGraphHeight / 2 + 30);


	//地面の接地判定
	if (pos_.y >= kGround)
	{
		pos_.y = kGround;//地面の位置に固定
		vel_.y = 0.0f;//速度を0に
		isGround_ = true;
		isJumping_ = false;
		canDoubleJumping_ = false;
	}

#ifdef _DEBUG
	if (input.IsTriggered("changeWeapon"))
	{
		printfDx("武器変えた!\n");
		int weponType = (static_cast<int>(currentBulletType_) + 1) % 2;
		currentBulletType_ = static_cast<PlayerBulletType>(weponType);
	}

#endif

	//弾の発射間隔がある場合はカウントダウン
	if (shotTimer_ > 0)shotTimer_--;

	//弾の発射・更新
	if (input.IsTriggered("shot") && shotTimer_ <= 0)
	{
		const auto& config = kBulletConfigs[
			static_cast<int>(currentBulletType_)];


		// 見た目の描画基準（プレイヤーの描画で使っている値）を使う
		float spawnBaseX = pos_.x + drawOffset_.x;
		float spawnBaseY = pos_.y + drawOffset_.y;

		//銃口オフセット：適宜調整してください（xは左右、yは上下）
		constexpr float gunOffsetX = 40.0f;
		constexpr float gunOffsetY = 20.0f;

		// 向きに応じた発射位置
		float spawnX = spawnBaseX + (isTurn_ ? gunOffsetX : -gunOffsetX);
		float spawnY = spawnBaseY + gunOffsetY;

		Vector2 spawnPos{ spawnX, spawnY };

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

	// プレイヤーの基準点（頭）
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
			idleH_, true, false, false);
	}
	else
	{
		DrawRectRotaGraph3(drawX, drawY,
			0, 0,
			kGraphWidth, kGraphHeight,
			kGraphWidth / 2, kGraphHeight / 2,
			1.5, 1.5,
			0.0,
			idleH_, true, true, false);
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
	//スクロールに応じた移動制限

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

//ダメージを受けたときの処理
void Player::OnDamage()
{
	isDamaged_ = true;
	damageTimer_ = kDamageDuration;
}
