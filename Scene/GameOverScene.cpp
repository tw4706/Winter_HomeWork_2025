#include "GameOverScene.h"
#include "SceneController.h"
#include "TitleScene.h"
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
		update_ = &GameOverScene::NormelUpdate;
		draw_ = &GameOverScene::NormelDraw;
	}
}

void GameOverScene::NormelUpdate(Input&input)
{
	if (input.IsTriggered("shot") ||
		input.IsTriggered("jump") ||
		input.IsTriggered("left") ||
		input.IsTriggered("right"))
	{
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
		controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
	}
}

void GameOverScene::FadeDraw()
{
	// GAME OVER の文字
	DrawString(400, 300, "GAME OVER", GetColor(255, 0, 0));

	// フェードをかける（黒い板を重ねる）
	int alpha = (frame_ * 255) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameOverScene::NormelDraw()
{
	DrawString(400, 300, "GAME OVER", GetColor(255, 0, 0));
	DrawString(360, 360, "Press any key to continue...", GetColor(255, 255, 255));
}

GameOverScene::GameOverScene(SceneController& controller):Scene(controller)
{
	update_ = &GameOverScene::FadeInUpdate;
	draw_ = &GameOverScene::FadeDraw;
}

void GameOverScene::Init()
{
	frame_ = kFadeDuration;
}

void GameOverScene::Update(Input& input)
{
	(this->*update_)(input);
}

void GameOverScene::Draw()
{
	(this->*draw_)();
}
