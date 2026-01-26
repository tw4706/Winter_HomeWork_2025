#include "DemoPlayer.h"
#include"Application.h"
#include<Dxlib.h>

namespace
{
	//=====================
	// 当たり判定・描画関連
	//=====================
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;
	constexpr int ColSize = 64;
	constexpr float kScale = 2.0f;
	constexpr float kDrawOffsetY = 60.0f;

	//=====================
	// アニメーション関連
	//=====================
	constexpr int kFrameCount = 8;
	constexpr int kFrameInterval = 6;

	//=====================
	// ステータス関連
	//=====================
	constexpr float kSpeed = 8.0f;

}

DemoPlayer::DemoPlayer(const Vector2& pos) :
	GameObject(pos, {0.0f,0.0f}, kGraphWidth, kGraphHeight, ColSize),
	idleGraphHandle_(-1),
	walkGraphHandle_(-1),
	stateTimer_(0),
	state_(DemoState::Idle)
{
	//重力は使わない
	SetUseGravity(false);
}

DemoPlayer::~DemoPlayer()
{
	DeleteGraph(idleGraphHandle_);
	DeleteGraph(walkGraphHandle_);
}

void DemoPlayer::Init()
{
	idleGraphHandle_ = LoadGraph("data/Player/Idle.png");
	walkGraphHandle_ = LoadGraph("data/Player/Walk.png");

	idleAnim_ = std::make_shared<Animation>(
		idleGraphHandle_,
		kGraphWidth, kGraphHeight,
		kFrameCount,			//フレーム数
		kFrameInterval,        //間隔
		kScale,
		true,
		0);
	walkAnim_ = std::make_shared<Animation>(
		walkGraphHandle_,
		kGraphWidth, kGraphHeight,
		kFrameCount,			//フレーム数
		kFrameInterval,        //間隔
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
		idleAnim_->Update();
		break;

	case DemoState::Walk:
		vel_.x = kSpeed;
		walkAnim_->Update();
		break;
	}

	pos_ += vel_;
}

void DemoPlayer::Draw()
{
	switch (state_)
	{
	case DemoState::Idle:
		idleAnim_->Draw(pos_.x, pos_.y- kDrawOffsetY, isTurn_);
		break;
	case DemoState::Walk:
		walkAnim_->Draw(pos_.x, pos_.y- kDrawOffsetY, isTurn_);
		break;
	}
}

void DemoPlayer::StartWalk()
{
	state_ = DemoState::Walk;
	vel_.x = kSpeed;
	stateTimer_ = 0;
	Application::GetInstance().GetSEManager().PlaySE(SE::PlayerWalk);
}