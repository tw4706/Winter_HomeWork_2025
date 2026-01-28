#include "Zombie.h"
#include "Player.h"
#include "Animation.h"
#include"GlobalConstants.h"
#include <Dxlib.h>
#include <cassert>
#include<cmath>

// エネミーに関する定数
namespace
{
	enum Graph
	{
		kIdleGraph,
		kWalkGraph,

		kGraphNum
	};

	const std::string kDogGraphName[kGraphNum] =
	{
		"data/Enemy/Zombie.png",
		"data/Enemy/zombie_walk.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kDogGraphName));

	//========================
	// アニメーション関連
	//========================
	//各状態遷移の総フレーム数
	constexpr int kIdleFrameCount = 11;
	constexpr int kWalkFrameCount = 6;
	//各状態遷移のフレーム間隔
	constexpr int kIdleFrameInterval = 6;
	constexpr int kWalkFrameInterval = 8;
	//状態ごとのフレーム数とフレーム間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kWalkFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval, kWalkFrameInterval };

	//========================
	// 当たり判定・描画関連
	//========================
	constexpr int kGraphWidth = 32;
	constexpr int kGraphColWidth = 24;
	constexpr int kGraphHeight = 48;
	constexpr int kGraphColHeight = 72;
	constexpr int kGraphHalfWidth = 32 / 2;
	constexpr int kGraphHalfHeight = 48 / 2;
	//敵の見た目のサイズ
	constexpr float kDrawW = kGraphWidth * 1.5f;
	constexpr float kDrawH = kGraphHeight * 1.5f;
	constexpr float kScale = 2.0f;					//拡大率
	//Y座標のオフセット
	constexpr int kPosYOffset = 20;
	constexpr int kPosYDrawOffset = 30;
	const float kDistance = 230.0f;					//プレイヤーとの距離
	constexpr float kIdleTriggerDistance = 250.0f;	//Idleアニメーション再生トリガー
	constexpr int kDefaultRedR = 255;
	constexpr int kDefaultRedG = 255;
	constexpr int kDefaultRedB = 255;

	//========================
	// ステータス関連
	//========================
	constexpr float kSpeed = 0.5f;					//エネミーの移動速度

	//========================
	// 被弾演出関連
	//========================
	constexpr int kDamageFlashInterval = 4;
	constexpr int kFlashAlpha = 128;
	constexpr int kFlashRedR = 255;
	constexpr int kFlashRedG = 64;
	constexpr int kFlashRedB = 64;
	constexpr int kFlashCycle = 2;
}

Zombie::Zombie(Vector2 pos, Vector2 vel) :
	Enemy(pos, vel),
	zombieState_(ZombieState::Idle),
	isInvincibled_(false),
	isIdleAnimPlayed_(false),
	idleReverseFrame_(0), 
	idleReverseTimer_(0),
	isReverseIdle_(false)
{

}

Zombie::~Zombie()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void Zombie::Init()
{
	isTurn_ = true;

	graphHandles_.resize(kGraphNum);
	animations_.resize(static_cast<int>(ZombieState::Walk) + 1);

	for (int i = 0; i < kGraphNum; i++)
	{
		bool isLoop = false;

		if (i == kWalkGraph)
		{
			isLoop = true;//Walk状態はループさせる
		}

		graphHandles_[i] = LoadGraph(kDogGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			isLoop,0);
	}

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y, kGraphColWidth, kGraphColHeight);
	zombieState_ = ZombieState::Idle;
	animations_[static_cast<int>(zombieState_)]->SetFrame(0);
	isIdleAnimPlayed_ = false;

	idleReverseFrame_ = 0;
	idleReverseTimer_ = 0;
	isReverseIdle_ = false;
}

void Zombie::Update()
{
	if (isDead_ || !pPlayer_) return;

	if (isDamageFlash_)
	{
		damageFlashTimer_--;
		if (damageFlashTimer_ <= 0)
		{
			isDamageFlash_ = false;
		}
	}

	UpdateAnim();

	//移動処理
	Move();

	Enemy::Update();

	//当たり判定の更新
	auto idleAnim = animations_[static_cast<int>(ZombieState::Idle)];

	//ゾンビがIdle状態でかつIdleアニメーションの最初のフレームの場合は当たり判定をなくす
	bool isBuried =(zombieState_ == ZombieState::Idle &&idleAnim->GetCurrentFrame() == 0);

	if (isBuried)
	{
		colRect_.SetCenter(pos_.x, pos_.y, 0, 0);
		isInvincibled_ = true;
	}
	else
	{
		colRect_.SetCenter(pos_.x, pos_.y - kPosYOffset, kGraphColWidth, kGraphColHeight);
		isInvincibled_ = false;
	}
}

void Zombie::Draw()
{
	Enemy::Draw();

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	bool isFlashRed = false;

	if (isDamageFlash_)
	{
		int t = damageFlashTimer_ / kDamageFlashInterval;
		isFlashRed = (t % kFlashCycle == 0);
	}

	if (isFlashRed)
	{
		//赤色で点滅
		SetDrawBlendMode(DX_BLENDMODE_ADD, kFlashAlpha);
		SetDrawBright(kFlashRedR, kFlashRedG, kFlashRedB);
	}

	animations_[static_cast<int>(zombieState_)]->Draw(drawX, drawY - kPosYDrawOffset, isTurn_);

	//点滅後元に戻す
	if (isFlashRed)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		SetDrawBright(kDefaultRedR, kDefaultRedG, kDefaultRedB);
	}
#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Zombie::UpdateAnim()
{
	if (!pPlayer_) return;

	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = std::abs(dx);

	auto idleAnim = animations_[static_cast<int>(ZombieState::Idle)];
	auto walkAnim = animations_[static_cast<int>(ZombieState::Walk)];

	//状態ごとの制御
	switch (zombieState_)
	{
	case ZombieState::Idle:
		if (!isIdleAnimPlayed_ && !isReverseIdle_ && distance < kIdleTriggerDistance)
		{
			idleAnim->Reset();
			isIdleAnimPlayed_ = true;
		}
		//Idleアニメーションが再生中ならUpdateする
		if (isIdleAnimPlayed_)
		{
			idleAnim->Update();

			//Idleアニメーションが完了したらWalk状態に遷移する
			if (idleAnim->IsAnimFinished() && distance < kDistance)
			{
				zombieState_ = ZombieState::Walk;
				walkAnim->Reset();
				isIdleAnimPlayed_ = false;
			}
		}
		//逆再生処理(アニメーションに入れると複数のキャラにも適用させないといけないので
		// ここで実装)
		if (isReverseIdle_)
		{
			idleReverseTimer_++;

			if (idleReverseTimer_ >= kIdleFrameInterval)
			{
				idleReverseTimer_ = 0;
				idleReverseFrame_--;

				if (idleReverseFrame_ <= 0)
				{
					idleReverseFrame_ = 0;
					isReverseIdle_ = false; //完全に潜った
				}

				idleAnim->SetFrame(idleReverseFrame_);
			}
		}
		break;

	case ZombieState::Walk:
		walkAnim->Update();

		//離れたらまた潜らせる
		if (distance >= kIdleTriggerDistance)
		{
			zombieState_ = ZombieState::Idle;

			isReverseIdle_ = true;
			isIdleAnimPlayed_ = false;

			idleReverseFrame_ = idleAnim->GetFrameCount() - 1;
			idleReverseTimer_ = 0;
			idleAnim->SetFrame(idleReverseFrame_);
		}
		break;
	}
}

void Zombie::Move()
{
	// プレイヤーがセットされていない場合は何もしない
	if (!pPlayer_) return;

	//Idle状態なら移動しない
	if (zombieState_ == ZombieState::Idle)
	{
		vel_.x = 0.0f;
		return;
	}

	//プレイヤーとの距離を見て移動する処理を追加
	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = std::abs(dx);

	if (distance < kDistance)
	{
		vel_.x = (dx > 0) ? kSpeed : (dx < 0 ? -kSpeed : 0);
		isTurn_ = (dx > 0) ? false : true;
	}
	else
	{
		vel_.x = 0.0f;
	}

#ifdef _DEBUG
	DrawFormatString(0, 80, GetColor(255, 255, 255), "ZombieDx:%f", dx);
	DrawFormatString(0, 100, GetColor(255, 255, 255), "ZombieDistance:%f", distance);
#endif
}
