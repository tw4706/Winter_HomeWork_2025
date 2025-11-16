#include<Dxlib.h>
#include<cassert>
#include "Player.h"
#include "Input.h"
#include"Bg.h"
#include "BulletManager.h"
#include"GlobalConstants.h"

// プレイヤーに関する定数
namespace
{
	//プレイヤーの画像サイズ
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;

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
	state_(PlayerState::IDLE),
	currentAnim_(nullptr)
{
	
}

Player::~Player()
{

}

void Player::Init()
{
	//初期化処理
	idleH_ = LoadGraph("data/Player/Idle.png");
	assert(idleH_ >= 0);
	attackH_ = LoadGraph("data/Player/Attack.png");
	assert(idleH_ >= 0);

	//アニメーションを状態ごとに設定
	idleAnim_ = std::make_shared<Animation>(idleH_, kGraphWidth, kGraphHeight);
	idleAnim_->InitIdle();
	SetAnimationState(PlayerState::IDLE, idleAnim_);
	attackAnim_ = std::make_shared<Animation>(attackH_, kGraphWidth, kGraphHeight);
	attackAnim_->InitAttack();
	SetAnimationState(PlayerState::ATTACK, attackAnim_);

	//初期に設定するアニメーション
	currentAnim_ = animMap_[PlayerState::IDLE];
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

	//地面の接地判定
	if (pos_.y >= kGround)
	{
		pos_.y = kGround;//地面の位置に固定
		vel_.y = 0.0f;//速度を0に
		isGround_ = true;
		isJumping_ = false;
		canDoubleJumping_ = false;
	}

	//弾の発射・更新
	if (input.IsTriggered("shot"))
	{
		SetAnimationState(PlayerState::ATTACK, attackAnim_);
		animMap_[PlayerState::ATTACK]->Reset();
		//三項演算子で向きに応じた弾の速度を設定
		Vector2 bulletVel_ = isTurn_ ? Vector2{ 10.0f,0.0f } : Vector2{ -10.0f,0.0f };
		auto bullet = std::make_shared<Bullet>(pos_, bulletVel_, BulletType::Player);
		//弾の初期化
		bullet->Init();
		//弾の追加
		bm.AddBullet(bullet);
	}
	if (damageTimer_ > 0)
	{
		damageTimer_--;
		if (damageTimer_ == 0)
		{
			isDamaged_ = false;
		}
	}

	//アニメーションの更新
	if(state_==PlayerState::ATTACK&&currentAnim_->
		GetCurrentFrame() == currentAnim_->GetFrameCount() - 1)
	{
		//攻撃アニメーションが終了したら待機状態に戻す
		SetAnimationState(PlayerState::IDLE, idleAnim_);
	}
	UpdateAnimation();

#ifdef _DEBUG
	//デバッグ用
	DrawFormatString(0, 0, GetColor(255, 255, 255), "PlayerX:%f", pos_.x);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "VelX:%f", vel_.x);
#endif


}

void Player::Draw()
{
	//アニメーションの描画
	DrawAnimation();

#ifdef _DEBUG
	if (isDamaged_)
	{
		//当たり判定の矩形の色を変える
		colRect_.Draw(0x0000ff, false);
	}
	else
	{
		colRect_.Draw(0xff0000, false);
	}
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

//アニメーション状態の設定
void Player::SetAnimationState(PlayerState state, std::shared_ptr<Animation> anim)
{
	animMap_[state] = anim;
	//
	if(animMap_.count(state))
	{
		//状態に応じたアニメーションを設定
		currentAnim_ = animMap_[state];
		state_ = state;
	}
}

//アニメーションの更新
void Player::UpdateAnimation()
{
	currentAnim_->Update();
}

//アニメーションの描画
void Player::DrawAnimation()
{
	float scrollX = pBg_->GetScrollX();
	currentAnim_->Draw(pos_,kGraphWidth,isTurn_,scrollX);
}
