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
	constexpr float kPercentageMax = 100.0f;

	//========================
	// 描画関連
	//========================
	//ゲームクリアの文字表示位置など
	constexpr int kCharWidth = 96;
	constexpr int kTextY = 200;
	constexpr int kScreenCenterX = 640;
	constexpr int kTextH = 100;
	constexpr int kShinigachiTextH = 340;
	constexpr int kBacktoTitleFrame = 30;
	constexpr int kBacktoTitleTextY = 500;
	constexpr int kFontSize = 64;
	constexpr int kFontTitleSize = 24;
	constexpr int kFadeAlpha = 255;
	constexpr int kShinigachiLogoFrame = 10;
	constexpr int kShigachiCharBuf = 64;

	//エフェクト表示位置のオフセット
	constexpr int kEffectOffsetY = 48;
	constexpr int kTextOffset = 4;
	const unsigned int kGameClearTextColor = GetColor(255, 240, 180);
	const unsigned int kShigachidoTextColor = GetColor(255, 180, 180);
}

ClearScene::ClearScene(SceneController& controller) :
	Scene(controller),
	frame_(0),
	bgHandle_(-1),
	fontHandle_(-1),
	fontTitleHandle_(-1),
	shinigachiTarget_(0),
	shinigachiFrame_(0),
	shinigachiCurrent_(0),
	isShinigachiRolling_(false),
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

	if (isShinigachiRolling_)
	{
		shinigachiFrame_++;

		//徐々に加速 → 減速
		int diff = shinigachiTarget_ - shinigachiCurrent_;
		int speed = (diff / kShinigachiLogoFrame > 1) ? (diff / kShinigachiLogoFrame) : 1;
		shinigachiCurrent_ += speed;

		if (shinigachiCurrent_ >= shinigachiTarget_)
		{
			shinigachiCurrent_ = shinigachiTarget_;
			isShinigachiRolling_ = false;
		}
	}

	if (input.IsTriggered("any_button"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
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
	int alpha = (frame_ * kFadeAlpha) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight,0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::NormalDraw()
{
	pEffectManager_->Draw();

	DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, bgHandle_, false);

	//=====ゲームクリア表示=====
	const char* clearText = "GAME CLEAR!";
	int clearTextW = GetDrawStringWidthToHandle(clearText, static_cast<int>(strlen(clearText)), fontHandle_);
	DrawStringToHandle(
		Game::kScreenWidth / 2 - clearTextW / 2 + kTextOffset,
		100 + kTextOffset,
		clearText,
		0x0000,
		fontHandle_);
	DrawStringToHandle(
		Game::kScreenWidth / 2 - clearTextW / 2,
		100,
		clearText,
		kGameClearTextColor,
		fontHandle_);

	//=====しにがち度=====
	char shiniBuf[kShigachiCharBuf];
	sprintf_s(shiniBuf, "しにがち度 : %d%%", shinigachiCurrent_);

	int shiniTextW = GetDrawStringWidthToHandle(
		shiniBuf,
		static_cast<int>(strlen(shiniBuf)),
		fontHandle_);

	DrawStringToHandle(
		Game::kScreenWidth / 2 - shiniTextW / 2 + kTextOffset,
		kShinigachiTextH + kTextOffset,
		shiniBuf,
		0x000000,
		fontHandle_);

	DrawStringToHandle(
		Game::kScreenWidth / 2 - shiniTextW / 2,
		kShinigachiTextH,
		shiniBuf,
		kShigachidoTextColor,
		fontHandle_);

	//=====タイトルに戻る=====
	bool visible = (frame_ / kBacktoTitleFrame) % 2 == 0;//30フレームごとに点滅

	if (visible)
	{
		const char* titleText = "Press any button to TitleScene";

		int titleTextW = GetDrawStringWidthToHandle(
			titleText,
			static_cast<int>(strlen(titleText)),
			fontTitleHandle_);

		DrawStringToHandle(
			Game::kScreenWidth / 2 - titleTextW / 2,
			kBacktoTitleTextY,
			titleText,
			0xffffff,
			fontTitleHandle_);
	}
}

void ClearScene::Init()
{
	frame_ = kFadeDuration;

	bgHandle_ = LoadGraph("data/map/bg.png");
	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", kFontSize, -1, -1);
	fontTitleHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", kFontTitleSize, -1, -1);
	clearText_ = "GAME CLEAR";
	charVisible_.assign(clearText_.size(), false);

	deathCount_ = controller_.GetProgress().deathCount_;
	playCount_ = controller_.GetProgress().playCount_;

	shinigachiTarget_ = (playCount_ > 0)
		? static_cast<int>(deathCount_ * kPercentageMax / playCount_): 0;

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