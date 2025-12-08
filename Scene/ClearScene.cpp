#include "ClearScene.h"
#include "TitleScene.h"
#include "SceneController.h"
#include"input.h"
#include<Dxlib.h>

namespace
{
	constexpr int kFadeDuration = 60;
}

ClearScene::ClearScene(SceneController& controller) :
	Scene(controller)
{
	update_ = &ClearScene::FadeInUpdate;
	draw_ = &ClearScene::FadeDraw;
}

void ClearScene::FadeInUpdate(Input&)
{
	frame_--;
	if (frame_ <= 0)
	{
		frame_ = 0;
		update_ = &ClearScene::NormalUpdate;
		draw_ = &ClearScene::NormalDraw;
	}
}
void ClearScene::NormalUpdate(Input& input)
{
	if (input.IsTriggered("shot") ||
		input.IsTriggered("jump") ||
		input.IsTriggered("left") ||
		input.IsTriggered("right"))
	{
		update_ = &ClearScene::FadeOutUpdate;
		draw_ = &ClearScene::FadeDraw;
		frame_ = 0;
	}
}
void ClearScene::FadeOutUpdate(Input&)
{
	frame_++;
	if (frame_ >= kFadeDuration)
	{
		controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
	}
}

void ClearScene::FadeDraw()
{
	DrawString(400, 300, "GAME CLEAR", GetColor(255, 0, 0));

	//フェード
	int alpha = (frame_ * 255) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, 1280, 720, GetColor(255, 255, 255), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::NormalDraw()
{
	DrawString(400, 300, "GAME CLEAR", GetColor(255, 0, 0));
	DrawString(360, 360, "Press any key to TitleScene", GetColor(255, 255, 255));
}

void ClearScene::Init()
{
	frame_ = kFadeDuration;
}

void ClearScene::Update(Input&input)
{
	(this->*update_)(input);
}

void ClearScene::Draw()
{
	(this->*draw_)();
}