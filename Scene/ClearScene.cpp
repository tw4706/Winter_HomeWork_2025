#include "ClearScene.h"
#include "TitleScene.h"
#include"Application.h"
#include"BGMManager.h"
#include"EffectManager.h"
#include"SpriteEffect.h"
#include "SceneController.h"
#include"input.h"
#include<Dxlib.h>

namespace
{
	constexpr int kFadeDuration = 60;

	//ゲームクリアの文字表示位置など
	constexpr int kFontSize = 96;
	constexpr int kCharWidth = 96;
	constexpr int kTextY = 200;
	constexpr int kScreenCenterX = 640;

	//エフェクト表示位置のオフセット
	constexpr int kEffectOffsetY = 48;
}

ClearScene::ClearScene(SceneController& controller) :
	Scene(controller),
	frame_(0),
	bgHandle_(-1),
	fontHandle_(-1),
	fontTitleHandle_(-1),
	currentTextIdx_(0),
	isTextEffectPlaying_(false)
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
	pEffectManager_->Update();

	if(input.IsTriggered("next") ||
		input.IsTriggered("shot")||
		input.IsTriggered("jump"))
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
	NormalDraw();

	//フェード
	int alpha = (frame_ * 255) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, 1280, 720, GetColor(255, 255, 255), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::NormalDraw()
{
	DrawExtendGraph(0, 0, 1280, 720, bgHandle_, false);

	int startX =kScreenCenterX -(static_cast<int>(clearText_.size()) * kCharWidth) / 2;

	for (size_t i = 0; i < clearText_.size(); ++i)
	{
		if (!charVisible_[i]) continue;

		char s[2] = { clearText_[i], '\0' };

		DrawStringToHandle(
			startX + static_cast<int>(i) * kCharWidth,
			kTextY,
			s,
			GetColor(255, 255, 0),
			fontHandle_);
	}

	pEffectManager_->Draw();

	if (currentTextIdx_ >= static_cast<int>(clearText_.size()))
	{
		DrawStringToHandle(360, 450,
			"Press any key to TitleScene",
			GetColor(255, 255, 255), fontTitleHandle_);
	}
}

void ClearScene::Init()
{
	frame_ = kFadeDuration;

	bgHandle_ = LoadGraph("data/map/bg.png");
	fontHandle_= CreateFontToHandle("g_コミックホラー悪党-教漢",96,-1,-1);
	fontTitleHandle_= CreateFontToHandle("g_コミックホラー悪党-教漢",48,-1,-1);
	clearText_ = "GAME CLEAR";
	charVisible_.assign(clearText_.size(), false);

	currentTextIdx_ = 0;
	isTextEffectPlaying_ = false;

	pEffectManager_ = std::make_shared<EffectManager>();

	Application::GetInstance().GetBGMManager().PlayBGM(BGM::GameClear);
}

void ClearScene::Update(Input&input)
{
	(this->*update_)(input);
}

void ClearScene::Draw()
{
	(this->*draw_)();
}