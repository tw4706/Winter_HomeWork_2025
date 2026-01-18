#include "DemoPlayer.h"
#include<Dxlib.h>

namespace
{
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;
	constexpr int ColSize = 64;
	constexpr float kScale = 1.5f;

	constexpr int kFrameCount = 8;
	constexpr int kFrameInterval = 6;

	constexpr float kSpeed = 2.0f;
	constexpr float kDrawOffsetY = 60.0f;
}

DemoPlayer::DemoPlayer(const Vector2& pos) :
	GameObject(pos, {0.0f,0.0f}, kGraphWidth, kGraphHeight, ColSize),
	graphHandle_(-1),
	stateTimer_(0),
	state_(DemoState::Idle)
{
	//重力は使わない
	SetUseGravity(false);
}

DemoPlayer::~DemoPlayer()
{
	DeleteGraph(graphHandle_);
}

void DemoPlayer::Init()
{
	graphHandle_ = LoadGraph("data/Player/Idle.png");
	anim_ = std::make_shared<Animation>(
		graphHandle_,
		kGraphWidth, kGraphHeight,
		kFrameCount,        // フレーム数
		kFrameInterval,        // 間隔
		kScale,
		true,
		0);
}

void DemoPlayer::Update()
{
	stateTimer_++;

	switch (state_)
	{
	case DemoState::Idle:
		vel_.x = 0.0f;
		break;

	case DemoState::Walk:
		vel_.x = kSpeed;
		break;
	}

	anim_->Update();
}

void DemoPlayer::Draw()
{
	anim_->Draw(pos_.x, pos_.y - kDrawOffsetY, isTurn_);
}

void DemoPlayer::StartWalk()
{
	state_ = DemoState::Walk;
	vel_.x = kSpeed;
	stateTimer_ = 0;
}
