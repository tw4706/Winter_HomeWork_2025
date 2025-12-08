#include "SelectScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SceneController.h"
#include"input.h"
#include<Dxlib.h>

namespace
{
	constexpr int kFadeDuration = 60;
}

SelectScene::SelectScene(SceneController& controller):
	Scene(controller),
	frame_(0),
	selectIndex_(0)
{
	update_ = &SelectScene::FadeInUpdate;
	draw_ = &SelectScene::FadeDraw;
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
	//選択肢の移動
	if (input.IsTriggered("up"))
	{
		selectIndex_--;
		if (selectIndex_ < 0) selectIndex_ = 1;
	}
	if (input.IsTriggered("down"))
	{
		selectIndex_++;
		if (selectIndex_ > 1) selectIndex_ = 0;
	}

	//決定
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
		if (selectIndex_ == 0) 
		{
			controller_.ChangeScene(std::make_shared<GameScene>(controller_));
		}
		else 
		{
			controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
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
	int baseX = 100;
	int y = 300;

	//Stage1
	int offsetStage1 = (selectIndex_ == 0) ? 20 : 0;   //選択中は右へずらす
	int colorStage1 = (selectIndex_ == 0) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

	if (selectIndex_ == 0)
	{
		DrawString(baseX - 40, y, "→", GetColor(255, 255, 0)); //矢印
	}
	DrawString(baseX + offsetStage1, y, "STAGE 1", colorStage1);


	//Title
	int offsetTitle = (selectIndex_ == 1) ? 20 : 0;    //選択中は右へずらす
	int colorTitle = (selectIndex_ == 1) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

	if (selectIndex_ == 1)
	{
		DrawString(baseX - 40, y + 60, "→", GetColor(255, 255, 0)); //矢印
	}
	DrawString(baseX + offsetTitle, y + 60, "TITLE", colorTitle);
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
