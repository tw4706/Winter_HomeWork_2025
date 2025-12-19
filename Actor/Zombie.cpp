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

	const std::string kGraphName[kGraphNum] =
	{
		"data/Enemy/Zombie.png",
		"data/Enemy/zombie_walk.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kGraphName));

	//グラフィックのサイズ
	constexpr int kGraphWidth = 32;
	constexpr int kGraphHeight = 48;
	constexpr int kGraphHalfWidth = 32 / 2;
	constexpr int kGraphHalfHeight = 48 / 2;

	//各状態遷移の総フレーム数
	constexpr int kIdleFrameCount = 11;
	constexpr int kWalkFrameCount = 6;

	//各状態遷移のフレーム間隔
	constexpr int kIdleFrameInterval = 6;
	constexpr int kWalkFrameInterval = 8;

	//状態ごとのフレーム数とフレーム間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kWalkFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval, kWalkFrameInterval };

	//敵の見た目のサイズ
	constexpr float kDrawW = kGraphWidth * 1.5f;
	constexpr float kDrawH = kGraphHeight * 1.5f;

	//エネミーの移動速度
	constexpr float kSpeed = 0.5f;

	//拡大率
	constexpr float kScale = 2.0f;

	//プレイヤーとの距離
	const float kDistance = 200.0f;

	constexpr int kPosYOffset = 20;
	constexpr int kPosYDrawOffset = 30;

	//Idleアニメーション再生トリガー
	constexpr float kIdleTriggerDistance = 270.0f;
}

Zombie::Zombie(Vector2 pos, Vector2 vel) :
	Enemy(pos, vel),
	zombieState_(ZombieState::Idle),
	isIdlePlayed_(false),
	isInvincibled_(false)
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
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			false,0
		);
	}

	//当たり判定の更新
	colRect_.SetCenter(pos_.x, pos_.y, kGraphWidth, kGraphHeight);
	zombieState_ = ZombieState::Idle;
}

void Zombie::Update()
{
	if (isDead_ || !pPlayer_) return;

	UpdateAnim();

	//移動処理
	Move();

	Enemy::Update();

	//当たり判定の更新
	auto idleAnim = animations_[static_cast<int>(ZombieState::Idle)];

	if (zombieState_ == ZombieState::Idle && !idleAnim->IsAnimFinished())
	{
		//潜っているときは当たり判定をなしにする
		colRect_.SetCenter(pos_.x, pos_.y, 0, 0);
		isInvincibled_ = true;
	}
	else
	{
		colRect_.SetCenter(pos_.x, pos_.y - kPosYOffset, kDrawW, kDrawH);
		isInvincibled_ = false;
	}
}

void Zombie::Draw()
{
	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	animations_[static_cast<int>(zombieState_)]->Draw(
		drawX, drawY - kPosYDrawOffset, isTurn_);

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

	switch (zombieState_)
	{
	case ZombieState::Idle:
		//Idleアニメーション終了かつプレイヤーが近づいたらWalkへ
		if (isIdlePlayed_ && idleAnim->IsAnimFinished() && distance < kDistance)
		{
			zombieState_ = ZombieState::Walk;
			walkAnim->Reset();
		}
		break;

	case ZombieState::Walk:
		//プレイヤーが離れたらIdleに戻す
		if (distance >= kDistance)
		{
			zombieState_ = ZombieState::Idle;
			isIdlePlayed_ = false;
		}
		break;
	}

	//Idleアニメーション制御
	if (zombieState_ == ZombieState::Idle)
	{
		auto idleAnim = animations_[static_cast<int>(ZombieState::Idle)];

		if (!isIdlePlayed_ && distance < kIdleTriggerDistance)
		{
			//まだ再生していない場合だけ Reset
			idleAnim->Reset();
			isIdlePlayed_ = true;
		}

		if (isIdlePlayed_)
		{
			// 再生中は Update
			if (!idleAnim->IsAnimFinished())
			{
				idleAnim->Update();
			}
			else
			{
				// 最後まで再生されたら停止
				idleAnim->SetFrame(idleAnim->GetFrameCount() - 1);
			}
		}
		else
		{
			// 遠い場合は最初のフレームで停止
			idleAnim->SetFrame(0);
		}

		// プレイヤーが遠くなったらフラグをリセット
		if (distance >= kIdleTriggerDistance)
		{
			isIdlePlayed_ = false;
		}
	}
	else if (zombieState_ == ZombieState::Walk)
	{
		walkAnim->Update();
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
	//DrawFormatString(0, 80, GetColor(255, 255, 255), "ZombieDx:%f", dx);
	//DrawFormatString(0, 100, GetColor(255, 255, 255), "ZombieDistance:%f", distance);
}

void Zombie::OnHit(int damage)
{
	//無敵ならダメージを受けない
	if (isInvincibled_) return;

	Enemy::OnHit(damage);
}
