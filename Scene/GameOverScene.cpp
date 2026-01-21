#include "GameOverScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "Application.h"
#include"Animation.h"
#include "Input.h"
#include"EffectManager.h"
#include"GlobalConstants.h"
#include <DxLib.h>
#include<EffekseerForDXLib.h>

namespace
{
	constexpr int kFadeDuration = 60;

	const char* kOptions[] = { "Restart", "Title" };
	const int kOptionCount = 2;

	//ゲームクリアの文字表示位置など
	constexpr int kFontSize = 96;
	constexpr int kCharWidth = 96;
	constexpr int kTextY = 100;
	constexpr int kScreenCenterX = 640;

	//エフェクト表示位置のオフセット
	constexpr int kEffectOffsetY = 48;
}

void GameOverScene::FadeInUpdate(Input&)
{
	frame_--;
	if (frame_ <= 0)
	{
		frame_ = 0;
		isSelecting_ = true;
		update_ = &GameOverScene::NormalUpdate;
		draw_ = &GameOverScene::NormalDraw;
	}
}

void GameOverScene::NormalUpdate(Input& input)
{
	pAnimation_->Update();
	pEffectManager_->Update();

	//GameOver演出
	if (currentTextIdx_ < static_cast<int>(gameOverText_.size()))
	{
		if (!isTextEffectPlaying_)
		{
			//空白はすぐに表示するようにする
			if (gameOverText_[currentTextIdx_] == ' ')
			{
				charVisible_[currentTextIdx_] = true;
				currentTextIdx_++;
				return;
			}

			int startX = kScreenCenterX -
				(static_cast<int>(gameOverText_.size()) * kCharWidth) / 2;

			Vector2 pos{ static_cast<float>(startX + currentTextIdx_ * kCharWidth + kCharWidth / 2),
					static_cast<float>(kTextY + kEffectOffsetY) };

			pEffectManager_->AddEffect(
				std::make_shared<SpriteEffect>(pos,
					"data/Effect/enemy_explosion.png",
					496, 16,
					16, 16,
					6, 2,
					6.0f));

			isTextEffectPlaying_ = true;
		}
		else
		{
			if (pEffectManager_->IsEmpty())
			{
				charVisible_[currentTextIdx_] = true;
				currentTextIdx_++;
				isTextEffectPlaying_ = false;
			}
		}
		return; //入力受付しない
	}

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
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameOverScene::NormalDraw()
{
	constexpr int text_offset = 4;

	DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, bgHandle_, false);
	DrawGraph(300, 560, deadCircleHandle_, true);

	pAnimation_->Draw(360, 500, false);

	// 画像を指定サイズに拡大/縮小して描画
	//DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, frameHandle_, TRUE);

	// 選択肢を描画
	for (int i = 0; i < kOptionCount; ++i)
	{
		int color = (i == selectIdx_) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

		DrawStringToHandle(550+ text_offset, 350 + i * 80+ text_offset, kOptions[i], GetColor(0,0,0), fontOptionHandle_);
		DrawStringToHandle(550, 350 + i * 80, kOptions[i], color, fontOptionHandle_);
	}

	DrawRotaGraph(500, 370 + selectIdx_ * 80, 2.0f, DX_PI / 2.0f, selectHandle_, true);

	int startX = kScreenCenterX - (static_cast<int>(gameOverText_.size()) * kCharWidth) / 2;

	for (size_t i = 0; i < gameOverText_.size(); ++i)
	{
		if (!charVisible_[i]) continue;

		char s[2] = { gameOverText_[i], '\0' };

		DrawStringToHandle(
			startX + static_cast<int>(i) * kCharWidth+ text_offset,
			kTextY+ text_offset,
			s,
			GetColor(0, 0, 0),
			fontHandle_);

		DrawStringToHandle(
			startX + static_cast<int>(i) * kCharWidth,
			kTextY,
			s,
			GetColor(255, 255, 0),
			fontHandle_);
	}

	pEffectManager_->Draw();
}

GameOverScene::GameOverScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	frame_(0),
	bgHandle_(-1),
	frameHandle_(-1),
	fontHandle_(-1),
	fontOptionHandle_(-1),
	selectHandle_(-1),
	deadCircleHandle_(-1),
	playerDeadGraphHandle_(-1),
	selectIdx_(0),
	isSelecting_(false),
	currentTextIdx_(0),
	isTextEffectPlaying_(false)
{
	update_ = &GameOverScene::FadeInUpdate;
	draw_ = &GameOverScene::FadeDraw;
}

void GameOverScene::Init()
{
	frame_ = kFadeDuration;

	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 96, -1, -1);
	fontOptionHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 48, -1, -1);

	bgHandle_ = LoadGraph("data/map/bg.png");
	frameHandle_ = LoadGraph("data/UI/Tutorialframe.png");
	selectHandle_ = LoadGraph("data/Bullet/Knife.png");
	deadCircleHandle_ = LoadGraph("data/UI/playerDeadCircle.png");
	playerDeadGraphHandle_ = LoadGraph("data/Player/Dead.png");
	pAnimation_ = std::make_shared<Animation>(
		playerDeadGraphHandle_,
		128, 128, 4, 10, 2.0f, false, 0);

	gameOverText_ = "GAME OVER";
	charVisible_.assign(gameOverText_.size(), false);

	currentTextIdx_ = 0;
	isTextEffectPlaying_ = false;

	pEffectManager_ = std::make_shared<EffectManager>();

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