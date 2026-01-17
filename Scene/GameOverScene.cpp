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

	const char* kOptions[] = { "Restart", "Title" };
	const int kOptionCount = 2;
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
	if (isSelecting_)
	{
		// 上下で選択
		if (input.IsTriggered("up"))
		{
			selectIdx_--;
			if (selectIdx_ < 0) selectIdx_ = kOptionCount - 1;
		}
		else if (input.IsTriggered("down"))
		{
			selectIdx_++;
			if (selectIdx_ >= kOptionCount) selectIdx_ = 0;
		}

		// 決定
		if (input.IsTriggered("next"))
		{
			Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
			isSelecting_ = false; // 選択終了
			update_ = &GameOverScene::FadeOutUpdate;
			draw_ = &GameOverScene::FadeDraw;
			frame_ = 0;
		}
	}
}

void GameOverScene::FadeOutUpdate(Input& input)
{
	frame_++;
	if (frame_ >= kFadeDuration)
	{
		if (selectIdx_ == 0) //リスタート
		{
			controller_.ChangeScene(std::make_shared<GameScene>(controller_, stageType_));
		}
		else //タイトルへ戻る
		{
			controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
		}
	}
}

void GameOverScene::FadeDraw()
{
	NormalDraw();

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

	if (frameHandle_ != -1)
	{
		// 画像を指定サイズに拡大/縮小して描画
		DrawExtendGraph(0, 0, 1280, 720, frameHandle_, TRUE);
	}

	DrawString(400, 300, "GAME OVER", GetColor(255, 0, 0));

	DrawString(360, 360, "Select an option:", GetColor(255, 255, 255));

	// 選択肢を描画
	for (int i = 0; i < kOptionCount; ++i)
	{
		int color = (i == selectIdx_) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
		DrawString(400, 400 + i * 40, kOptions[i], color);
	}

}

GameOverScene::GameOverScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	frame_(0),
	bgHandle_(-1),
	frameHandle_(-1),
	deadCircleHandle_(-1),
	playerDeadGraphHandle_(-1),
	selectIdx_(0),
	isSelecting_(false)
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