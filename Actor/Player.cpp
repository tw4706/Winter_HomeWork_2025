#include "Player.h"
#include "Input.h"
#include"Bullet.h"
#include"Bg.h"
#include"StageType.h"
#include"GameProgress.h"
#include "BulletManager.h"
#include "CollisionManager.h"
#include"GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>
#include<string>

//プレイヤーに関する定数
namespace
{
	enum Graph
	{
		kIdleGraph,
		kAttackgraph,
		kWalkGraph,
		kJumpGraph,
		kHurtGraph,
		kDeathGraph,

		kGraphNum
	};

	const std::string kGraphName[kGraphNum] =
	{
		"data/Player/Idle.png",
		"data/Player/Attack_correction.png",
		"data/Player/Walk.png",
		"data/Player/Jump1.png",
		"data/Player/Hurt.png",
		"data/Player/Dead.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kGraphName));

	//プレイヤーの画像サイズ
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;
	constexpr int kGraphHalfWidth = 128 / 2;
	constexpr int kGraphHalfHeight = 128 / 2;
	constexpr float kGraphColSize = 64.0f;
	constexpr int kMaxHp = 2;

	//拡大率
	constexpr float kScale = 1.5f;
	//移動速度
	constexpr float kSpeed = 5.0f;

	//ジャンプ時の横移動速度
	constexpr float kHalfSpeed = 2.5f;

	//ジャンプの高さ
	constexpr float kJumpPower = 12.0f;

	//ダブルジャンプの高さ
	constexpr float kDoubleJumpPower = 10.0f;

	//落下判定となる座標
	constexpr float kFallLimit = 2100.0f;

	//弾の種類
	constexpr int kBulletNum = 3;

	//弾がプレイヤーから出る位置のオフセット
	constexpr float kGunOffsetX = 40.0f;
	constexpr float kGunOffsetY = 10.0f;

	//たいまつの投げる位置のオフセット
	constexpr float kTorchFireOffsetY = 50.0f;

	//攻撃をしている間の時間
	constexpr int kAttackDuration = 30;

	//ダメージを受けたときの無敵時間
	constexpr int kDamageDuration = 60;

	//被弾した時のノックバックの強さ
	constexpr float kKnockBackX = 10.0f;
	constexpr float kKnockBackY = 5.0f;
	constexpr float kKnockBackSpeed = 0.94f;

	//重力
	constexpr float kGravity = 1.0f;

	//当たり判定の調整用
	constexpr int kColYOffset = 32;

	//描画の調整用
	constexpr int kColPosYOffset = 15;
	constexpr int kPosYOffset = 60;
	constexpr float kWalkColXOffset = 15.0f;

	//各状態遷移の総フレーム数
	constexpr int kIdleFrameCount = 8;
	constexpr int kAttackFrameCount = 7;
	constexpr int kWalkFrameCount = 8;
	constexpr int kJumpFrameCount = 8;
	constexpr int kHurtFrameCount = 4;
	constexpr int kDeathFrameCount = 4;

	//攻撃・ジャンプのアニメーションはさらに部分だけ切り取る
	constexpr int kAttackStartFrame = 3;
	constexpr int kAttackEndFrame = 8;
	constexpr int kJumpStartFrame = 3;
	constexpr int kJumpEndFrame = 8;

	//各状態遷移のフレーム間隔
	constexpr int kIdleFrameInterval = 6;
	constexpr int kAttackFrameInterval = 5;
	constexpr int kWalkFrameInterval = 5;
	constexpr int kJumpFrameInterval = 8;
	constexpr int kHurtFrameInterval = 6;
	constexpr int kDeathFrameInterval = 6;

	//状態ごとのフレーム数とフレーム間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kAttackFrameCount,kWalkFrameCount, kJumpFrameCount, kHurtFrameCount, kDeathFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval,
		kAttackFrameInterval, kWalkFrameInterval, kJumpFrameInterval, kHurtFrameInterval, kDeathFrameInterval }; // 更新速度
}

Player::Player(Vector2 pos, Vector2 vel) :
	GameObject(pos, vel, kGraphWidth, kGraphHeight, kGraphColSize),
	initializePos_{ pos },
	hp_(kMaxHp),
	isJumping_(false),
	isDoubleJumping_(false),
	isDamaged_(false),
	isTouching_(false),
	isAttacking_(false),
	attackTimer_(0),
	damageTimer_(0),
	shotTimer_(0),
	isAlive_(true),
	isDeathAnimFinished_(false),
	autoWalkDir_(1),
	autoWalkSpeed_(kSpeed),
	currentStage_(StageType::Stage1),
	controlMode_(PlayerControl::Normal),
	isUnlockedTorch_(false),
	state_(PlayerState::Idle),
	prevState_(PlayerState::Idle),
	currentBulletType_(BulletType::Knife),
	gameProgress_(nullptr)
{
	isTurn_ = true;
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
	animations_.resize(kGraphNum);

	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			false, 0);
	}
}

void Player::Update(Input& input, BulletManager& bm,StageType stage)
{
	currentStage_ = stage;

	//武器を選択する時に停止させる
	if (controlMode_ == PlayerControl::Stop)
	{
		vel_ = { 0.0f,0.0f };
		state_ = PlayerState::Idle;
		animations_[static_cast<int>(state_)]->Update();
		return;
	}

	GameObject::Update();

	float colOffsetX = 0.0f;

	if (state_ == PlayerState::Walk)
	{
		colOffsetX = isTurn_ ? -kWalkColXOffset : kWalkColXOffset;
	}

	float colX = pos_.x + colOffsetX;

	//ボス1撃破後の自動で移動する際の処理
	if (controlMode_ == PlayerControl::AutoWalking)
	{
		vel_.x = autoWalkSpeed_ * autoWalkDir_;
		vel_.y += kGravity;

		colRect_.SetCenter(
			colX,pos_.y - kColPosYOffset,
			kGraphHalfWidth,
			kGraphHeight - kColYOffset);

		state_ = PlayerState::Walk;
		animations_[static_cast<int>(state_)]->Update();
		return;
	}

	colRect_.SetCenter(colX, pos_.y - kColPosYOffset,
		kGraphHalfWidth, kGraphHeight - kColYOffset);

	//状態遷移の更新
	UpdateState(input);

	//死亡処理
	if (!isAlive_)
	{
		animations_[static_cast<int>(PlayerState::Death)]->Update();

		//死亡アニメーションが終了したらゲームオーバー処理を呼び出す
		if (animations_[static_cast<int>(PlayerState::Death)]->IsAnimFinished() && onGameOver_)
		{
			onGameOver_();
		}
		return;
	}

	//移動処理
	Move(input);

	// ジャンプ処理
	Jump(input);

	//発射処理
	Shot(input, bm);

	//状態がかわったらアニメーションをリセットする
	if (state_ != prevState_)
	{
		auto anim = animations_[static_cast<int>(state_)];
		anim->Reset();

		if (state_ == PlayerState::Jump)
		{
			anim->Setloop(false);
			anim->SetFrame(kJumpStartFrame);
		}
		else if (state_ == PlayerState::Attack)
		{
			anim->Setloop(false);
			anim->SetFrame(kAttackStartFrame);
		}
		else
		{
			anim->Setloop(true);
		}

		prevState_ = state_;
	}

	//アニメーションの更新
	animations_[static_cast<int>(state_)]->Update();

	//落下判定
	if (pos_.y > kFallLimit)
	{
		if (onGameOver_)
		{
			onGameOver_();
		}
		return;
	}

	//ノックバック
	if (isDamaged_)
	{
		vel_.x *= kKnockBackSpeed;
	}

	//無敵時間
	if (damageTimer_ > 0)
	{
		damageTimer_--;
		if (damageTimer_ == 0)
		{
			isDamaged_ = false;
			vel_.x = 0.0f;
		}
	}

	//ダメージ中は移動や攻撃を受け付けない
	if (isDamaged_)
	{
		animations_[static_cast<int>(state_)]->Update();
		return;
	}
	//武器の切り替え
	if (input.IsTriggered("changeWeapon"))
	{
		int next = static_cast<int>(currentBulletType_);

		//次の武器への切り替え
		for (int i = 0; i < kBulletNum; i++)
		{
			next = (next + 1) % kBulletNum;

			//たいまつが未解放ならスキップ
			if (static_cast<BulletType>(next) == BulletType::Torch &&
				!isUnlockedTorch_)
			{
				continue;
			}

			currentBulletType_ = static_cast<BulletType>(next);
			break;
		}
	}

#ifdef _DEBUG
	//デバッグ用
	DrawFormatString(0, 0, GetColor(255, 255, 255), "PlayerX:%f", pos_.x);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "PlayerY:%f", pos_.y);
#endif
}

void Player::Draw()
{
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	//描画
	int animIndex = static_cast<int>(state_);
	int frame = animations_[animIndex]->GetFrameCount();

	animations_[animIndex]->Draw(drawX, drawY - kPosYOffset, !isTurn_);

#ifdef _DEBUG
	//当たり判定の矩形の色を変える
	colRect_.DrawAndCamera(cameraOffset_, isDamaged_ ? 0x0000ff : 0xff0000, false);
	DrawFormatString(
		20,
		20,
		GetColor(255, 255, 255),
		"Player X : %.1f",
		pos_.x);
#endif
}

//移動処理
void Player::Move(Input& input)
{
	//ダメージ中は移動できない
	if (isDamaged_)
	{
		return; // 横速度を上書きしない
	}

	bool CanJumpMove = isGround_ || isDoubleJumping_;
	//地面にいるときかつダブルジャンプが可能な時
	if (CanJumpMove)
	{
		if (input.IsTriggered("left") || input.IsTriggered("right"))
		{
			OnTutorialAction(TutorialAction::Move);
		}
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
	if (input.IsTriggered("jump"))
	{
		// 通常ジャンプ
		if (isGround_)
		{
			vel_.y = -kJumpPower;
			isGround_ = false;
			isDoubleJumping_ = true;
			OnTutorialAction(TutorialAction::Jump);
			return;
		}

		//二段ジャンプ
		if (isDoubleJumping_)
		{
			vel_.y = -kDoubleJumpPower;
			isDoubleJumping_ = false;
			OnTutorialAction(TutorialAction::DoubleJump);
		}
	}
}

void Player::Shot(Input& input, BulletManager& bm)
{
	if (shotTimer_ > 0)
	{
		shotTimer_--;
	}

	//攻撃処理
	if (input.IsTriggered("shot") && shotTimer_ <= 0)
	{
		isAttacking_ = true;
		attackTimer_ = kAttackDuration;

		auto anim = animations_[static_cast<int>(PlayerState::Attack)];
		anim->Reset();
		anim->Setloop(true);
		anim->SetFrame(kAttackStartFrame);

		//たいまつが使えないステージなら発射出来ないようにする
		if (currentBulletType_ == BulletType::Torch && !IsUnlockedTorch())
		{
			return;
		}

		int idx = static_cast<int>(currentBulletType_);
		assert(idx >= 0 && idx < kBulletNum);
		const auto& config = kBulletConfigs[idx];

		//弾の発射される位置
		float spawnX = pos_.x + (isTurn_ ? kGunOffsetX : -kGunOffsetX);
		float spawnY = pos_.y - kGunOffsetY;

		if (currentBulletType_ == BulletType::Torch)
		{
			spawnY -= kTorchFireOffsetY;
		}

		Vector2 spawnPos = { spawnX, spawnY };

		//弾の速度
		Vector2 bulletVel = isTurn_ ?
			Vector2{ config.speed, 0.0f } : Vector2{ -config.speed, 0.0f };

		//弾の生成
		auto bullet = std::make_shared<Bullet>(spawnPos, bulletVel, currentBulletType_, pBg_);
		bullet->Init();
		bullet->SetBg(pBg_);
		bullet->SetDirection(isTurn_);
		bm.Init(bullet);

		OnTutorialAction(TutorialAction::Attack);

		// 攻撃開始
		isAttacking_ = true;
		animations_[static_cast<int>(PlayerState::Attack)]->Reset();
		shotTimer_ = config.shotInterval;
	}
}

//ダメージを受けたときの処理
void Player::OnDamage(float enemyX)
{
	if (controlMode_ == PlayerControl::AutoWalking) return;
	if (!isAlive_) return;
	if (isDamaged_) return;

	hp_--;
	OnTutorialAction(TutorialAction::Damaged);

	if(hp_ <= 0)
	{
		Dead();
		return;
	}

	isDamaged_ = true;
	damageTimer_ = kDamageDuration;

	//向きに応じて横方向のノックバック
	if (pos_.x < enemyX)
	{
		vel_.x = -kKnockBackX;
	}
	else
	{
		vel_.x = kKnockBackX;
	}

	//上方向にノックバック
	vel_.y = -kKnockBackY;

	//空中にいる判定にする
	isGround_ = false;

	//ダメージアニメーションへ
	animations_[static_cast<int>(PlayerState::Hurt)]->Reset();

}

void Player::Dead()
{
	if (!isAlive_)return;

	isAlive_ = false;
	state_ = PlayerState::Death;
	animations_[static_cast<int>(state_)]->Reset();
}

//各アニメーションの処理
void Player::UpdateState(Input& input)
{
	//死亡
	if (!isAlive_)
	{
		state_ = PlayerState::Death;
		auto deathAnim = animations_[static_cast<int>(PlayerState::Death)];
		isDeathAnimFinished_ = deathAnim->IsAnimFinished();
		return;
	}

	//ダメージを受けているならダメージ状態
	if (isDamaged_)
	{
		state_ = PlayerState::Hurt;
		return;
	}

	//攻撃中なら攻撃状態
	if (isAttacking_)
	{
		state_ = PlayerState::Attack;

		attackTimer_--;
		if (attackTimer_ <= 0)
		{
			isAttacking_ = false;
		}
		return;
	}

	//ジャンプ中ならジャンプ状態
	if (!isGround_)
	{
		state_ = PlayerState::Jump;
		return;
	}

	//移動状態の更新
	if (vel_.x != 0)
	{
		state_ = PlayerState::Walk;
	}
	else//そうでないなら待機状態
	{
		state_ = PlayerState::Idle;
	}
}

void Player::StartAutoWalk(int dir)
{
	controlMode_ = PlayerControl::AutoWalking;
	autoWalkDir_ = dir;
	vel_.x = kSpeed * dir;
	isTurn_ = (dir > 0);

	isAttacking_ = false;
	isDamaged_ = false;

	state_ = PlayerState::Walk;
	animations_[static_cast<int>(state_)]->Reset();
}

void Player::OnTutorialAction(TutorialAction action)
{
	if (!gameProgress_) return;

	switch (action)
	{
	case TutorialAction::Move:
		gameProgress_->tutorialMoved_ = true;
		break;
	case TutorialAction::Jump:
		gameProgress_->tutorialJumped_ = true;
		break;
	case TutorialAction::DoubleJump:
		gameProgress_->tutorialDoubleJumped_ = true;
		break;
	case TutorialAction::Attack:
		gameProgress_->tutorialAttacked_ = true;
		break;
	}
}
