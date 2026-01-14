#include "GameOverScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "Application.h"
#include"Animation.h"
#include "Input.h"
#include <DxLib.h>

namespace
{
	constexpr int kFadeDuration = 60;
}

void GameOverScene::FadeInUpdate(Input&)
{
	frame_--;
	if (frame_ <= 0)
	{
		frame_ = 0;
		update_ = &GameOverScene::NormalUpdate;
		draw_ = &GameOverScene::NormalDraw;
	}
}

void GameOverScene::NormalUpdate(Input& input)
{
	pAnimation_->Update();
	if (input.IsTriggered("next"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
		update_ = &GameOverScene::FadeOutUpdate;
		draw_ = &GameOverScene::FadeDraw;
		frame_ = 0;
	}
}

void GameOverScene::FadeOutUpdate(Input& input)
{
	frame_++;
	if (frame_ >= kFadeDuration)
	{
		controller_.ChangeScene(std::make_shared<GameScene>(controller_, stageType_));
		return;
	}
}

void GameOverScene::FadeDraw()
{
	DrawString(400, 300, "GAME OVER", GetColor(255, 0, 0));

	//フェード
	int alpha = (frame_ * 255) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameOverScene::NormalDraw()
{
	DrawExtendGraph(0, 0, 1280, 720, bgHandle_, false);
	DrawGraph(300, 560, deadCircleHandle_, true);

	pAnimation_->Draw(360, 500, false);
	// フレーム表示座標とサイズ
	const int frameX = 300;
	const int frameY = 300;
	const int frameWidth = 680;  // 好きな横幅に変更
	const int frameHeight = 150; // 好きな縦幅に変更

	if (frameHandle_ != -1)
	{
		// 画像を指定サイズに拡大/縮小して描画
		DrawExtendGraph(0, 0, 1280, 720, frameHandle_, TRUE);
	}

	DrawString(400, 300, "GAME OVER", GetColor(255, 0, 0));
	DrawString(360, 360, "Press any key to restart...", GetColor(255, 255, 255));
	//DrawGraph(360, 500, playerDeadGraphHandle_, true);
}

GameOverScene::GameOverScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	bgHandle_(-1),
	frameHandle_(-1),
	deadCircleHandle_(-1),
	playerDeadGraphHandle_(-1)
{
	update_ = &GameOverScene::FadeInUpdate;
	draw_ = &GameOverScene::FadeDraw;
}

void GameOverScene::Init()
{
	frame_ = kFadeDuration;

	bgHandle_ = LoadGraph("data/map/bg.png");
	frameHandle_ = LoadGraph("data/UI/Tutorialframe.png");
	deadCircleHandle_ = LoadGraph("data/UI/playerDeadCircle.png");
	playerDeadGraphHandle_ = LoadGraph("data/Player/Dead.png");
	pAnimation_ = std::make_shared<Animation>(
		playerDeadGraphHandle_,
		128, 128,
		4, 10,
		2.0f, false, 0);

	//BGM再生
	Application::GetInstance().GetBGMManager().PlayBGM(BGM::GameOver);
}

void GameOverScene::Update(Input& input)
{
	(this->*update_)(input);
}

void GameOverScene::Draw()
{
	(this->*draw_)();
}