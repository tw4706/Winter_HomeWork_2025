#include "DemoZombie.h"
#include "Animation.h"
#include <Dxlib.h>
#include <cassert>

namespace
{
	enum Graph
	{
		kIdleGraph,
		kWalkGraph,

		kGraphNum
	};

	const std::string kDogGraphName[kGraphNum]=
	{
		"data/Enemy/Zombie.png",
		"data/Enemy/zombie_walk.png"
	};
	static_assert(static_cast<int>(kGraphNum) == _countof(kDogGraphName));

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
	constexpr float kScale = 3.0f;

	//Y座標のオフセット
	constexpr int kPosYOffset = 20;
	constexpr int kPosYDrawOffset = 30;
}

DemoZombie::DemoZombie(Vector2 pos):
	Enemy(pos,vel_),
	zombieState_(DemoZombieState::Idle)
{
	vel_ = Vector2{ 0.0f,0.0f };
}

DemoZombie::~DemoZombie()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void DemoZombie::Init()
{
	isTurn_ = true;

	graphHandles_.resize(kGraphNum);
	animations_.resize(static_cast<int>(DemoZombieState::Walk) + 1);

	for (int i = 0; i < kGraphNum; i++)
	{
		bool isLoop = false;
		if (i == kWalkGraph)
		{
			isLoop = true;
		}
		graphHandles_[i] = LoadGraph(kDogGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale, isLoop, 0);
	}

	colRect_.SetCenter(pos_.x, pos_.y, kGraphWidth, kGraphHeight);
	zombieState_ = DemoZombieState::Walk;
	animations_[static_cast<int>(zombieState_)]->SetFrame(0);
}

void DemoZombie::Update()
{
	UpdateAnim();
	Move();

	pos_.x += vel_.x;
}

void DemoZombie::Draw()
{
	animations_[static_cast<int>(zombieState_)]->Draw(pos_.x, pos_.y, isTurn_);
}

void DemoZombie::Move()
{
	vel_.x = -kSpeed;
}

void DemoZombie::UpdateAnim()
{
	auto idleAnim = animations_[static_cast<int>(DemoZombieState::Idle)];
	auto walkAnim = animations_[static_cast<int>(DemoZombieState::Walk)];

	switch (zombieState_)
	{
	case DemoZombieState::Idle:
		idleAnim->Update();
		break;
	case DemoZombieState::Walk:
		walkAnim->Update();
		break;
	}
}
