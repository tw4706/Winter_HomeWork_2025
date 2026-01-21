#include "ClearScene.h"
#include "TitleScene.h"
#include"Application.h"
#include"BGMManager.h"
#include"EffectManager.h"
#include"SpriteEffect.h"
#include "SceneController.h"
#include "GlobalConstants.h"
#include"input.h"
#include<Dxlib.h>
#include <cmath>

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

	constexpr int kClearTextOffset = 4;
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

	frame_++;

	if (input.IsTriggered("next") ||
		input.IsTriggered("shot") ||
		input.IsTriggered("jump"))
	{
		controller_.GetProgress().Reset();//死亡回数をリセットする
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
	pEffectManager_->Draw();

	DrawExtendGraph(0, 0, 1280, 720, bgHandle_, false);

	const char* clearText = "GAME CLEAR!";
	int clearTextW = GetDrawStringWidthToHandle(clearText, static_cast<int>(strlen(clearText)), fontHandle_);
	DrawStringToHandle(
		Game::kScreenWidth / 2 - clearTextW / 2 + kClearTextOffset,
		100 + kClearTextOffset,
		clearText,
		0x0000,
		fontHandle_);
	DrawStringToHandle(
		Game::kScreenWidth / 2 - clearTextW / 2,
		100,
		clearText,
		GetColor(255, 240, 180),
		fontHandle_);


	bool visible = (frame_ / 30) % 2 == 0;//30フレームごとに点滅

	if (visible)
	{
		DrawStringToHandle(
			360, 500,
			"Press any button to TitleScene",
			GetColor(255, 255, 255),
			fontTitleHandle_);
	}
}

void ClearScene::Init()
{
	frame_ = kFadeDuration;

	bgHandle_ = LoadGraph("data/map/bg.png");
	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 64, -1, -1);
	fontTitleHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 24, -1, -1);
	clearText_ = "GAME CLEAR";
	charVisible_.assign(clearText_.size(), false);

	deathCount_ = controller_.GetProgress().deathCount_;
	playCount_ = controller_.GetProgress().playCount_;

	shinigachiTarget_ = (playCount > 0)
		? static_cast<int>(deathCount_ * 100.0f / playCount)
		: 0;

	shinigachiCurrent_ = 0;
	shinigachiFrame_ = 0;
	isShinigachiRolling_ = true;

	currentTextIdx_ = 0;
	isTextEffectPlaying_ = false;

	pEffectManager_ = std::make_shared<EffectManager>();

	Application::GetInstance().GetBGMManager().PlayBGM(BGM::GameClear);
}

void ClearScene::Update(Input& input)
{
	(this->*update_)(input);
}

void ClearScene::Draw()
{
	(this->*draw_)();
}