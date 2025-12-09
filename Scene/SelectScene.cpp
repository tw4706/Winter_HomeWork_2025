#include "SelectScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SceneController.h"
#include"input.h"
#include<Dxlib.h>

namespace
{
	constexpr int kFadeDuration = 60;
	constexpr int kSelectIdxX = 20;
	constexpr int kDrawOffsetX = 40;
}

SelectScene::SelectScene(SceneController& controller) :
	Scene(controller),
	frame_(0),
	selectIndex_(0)
{
	update_ = &SelectScene::FadeInUpdate;
	draw_ = &SelectScene::FadeDraw;
	stageMenu_ =
	{
		"STAGE 1",
		"BOSS DEBUG",
		"TITLE"
	};
}

void SelectScene::FadeInUpdate(Input&)
{
	frame_--;
	if (frame_ <= 0)
	{
		frame_ = 0;
		update_ = &SelectScene::NormalUpdate;
		draw_ = &SelectScene::NormalDraw;
	}
}

void SelectScene::NormalUpdate(Input& input)
{
	int maxIndex = (int)stageMenu_.size();

	//‘I‘ðŽˆ‚ÌˆÚ“®
	if (input.IsTriggered("up"))
	{
		selectIndex_--;
		if (selectIndex_ < 0)
		{
			selectIndex_ = maxIndex-1;
		}
	}
	if (input.IsTriggered("down"))
	{
		selectIndex_++;
		if (selectIndex_ >= maxIndex)
		{
			selectIndex_ = 0;
		}
	}

	//Œˆ’è
	if (input.IsTriggered("ok"))
	{
		update_ = &SelectScene::FadeOutUpdate;
		draw_ = &SelectScene::FadeDraw;
	}
}
void SelectScene::FadeOutUpdate(Input&)
{
	frame_++;
	if (frame_ >= kFadeDuration)
	{
		std::string item = stageMenu_[selectIndex_];

		if (item == "TITLE")
		{
			controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
		}
		else if (item == "BOSS DEBUG")
		{
			controller_.ChangeScene(std::make_shared<GameScene>(controller_, StageType::BossDebugStage));
		}
		else if (item == "STAGE 1")
		{
			controller_.ChangeScene(std::make_shared<GameScene>(controller_, StageType::Stage1));
		}
	}
}

void SelectScene::FadeDraw()
{
	float rate = static_cast<float>(frame_) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
	DrawBox(0, 0, 1280, 720, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SelectScene::NormalDraw()
{
	int drawX = 100;
	int drawY = 100;
	int pitch = 30;

	for (int i = 0; i < stageMenu_.size(); i++)
	{
		bool isSelected = (i == selectIndex_);

		int y = drawY + pitch * i;

		int offsetX = isSelected ? kSelectIdxX : 0;

		int color = isSelected ? 0x00ffff : 0xffffff;

		if (isSelected)
		{
			DrawString(drawX - kDrawOffsetX, y, "¨", 0x00ffff);
		}
		DrawString(drawX + offsetX, y,stageMenu_[i].c_str(), color);
	}
}

void SelectScene::Init()
{
	frame_ = kFadeDuration;
	selectIndex_ = 0;
}

void SelectScene::Update(Input& input)
{
	(this->*update_)(input);
}

void SelectScene::Draw()
{
	(this->*draw_)();
}
