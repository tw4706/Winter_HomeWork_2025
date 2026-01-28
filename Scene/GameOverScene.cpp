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

	//=======================
	// 描画関連
	//=======================
	//ゲームクリアの文字表示位置など
	constexpr int kFontSize = 96;
	constexpr int kFontOptionSize = 48;
	constexpr int kCharWidth = 96;
	constexpr int kTextY = 100;
	constexpr int kScreenCenterX = 640;

	//プレイヤーの死亡アニメーション用
	constexpr int kCircleX = 300;
	constexpr int kCircleY = 560;
	constexpr int kPlayerX = 360;
	constexpr int kPlayerY = 500;

	//文字の揺らし演出用
	constexpr int kShakeRange = 6;
	constexpr float kShakeSpeed = 0.1f;
	constexpr float kShakePhase = 0.8f;
	constexpr int kShadowOffset = 4;

	//エフェクト表示位置のオフセット
	constexpr int kEffectOffsetY = 48;
	constexpr int kGraphWidth = 128;
	constexpr int kGraphHeight = 128;
	constexpr int kFrameCount = 4;
	constexpr int kFrameInterval = 10;
	constexpr float kScale = 2.0f;
	constexpr int kFadeMaxAlpha = 255;

	//オプションの表示位置調整
	constexpr int kOptionBaseX = 550;
	constexpr int kOptionBaseY = 350;
	constexpr int kOptionIntervalY = 80;

	//=======================
	// カーソル関連
	//=======================
	constexpr int kCursorX = 500;
	constexpr int kCursorBaseY = 370;
	constexpr float kCursorScale = 2.0f;
	constexpr float kCursorAngle = DX_PI_F / 2.0f;

	constexpr float kCursorShakeSpeed = 0.15f;
	constexpr float kCursorShakeRange = 5.0f;
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
	frame_++;

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
	}

	if (isSelecting_)
	{
		//上下で選択
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

		//
		// 決定
		if (input.IsTriggered("any_button"))
		{
			Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
			isSelecting_ = false;
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
			controller_.GetProgress().playCount_++;
			controller_.ChangeScene(std::make_shared<GameScene>(controller_, stageType_));
		}
		else //タイトルへ戻る
		{
			controller_.GetProgress().Reset();
			controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
		}
	}
}

void GameOverScene::FadeDraw()
{
	NormalDraw();

	//フェード
	int alpha = (frame_ * kFadeMaxAlpha) / kFadeDuration;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameOverScene::NormalDraw()
{
	DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, bgHandle_, false);
	DrawGraph(kCircleX, kCircleY, deadCircleHandle_, true);

	pAnimation_->Draw(kPlayerX, kPlayerY, false);

	// 選択肢を描画
	for (int i = 0; i < kOptionCount; ++i)
	{
		int color = (i == selectIdx_) ? 0xffff00 :0xffffff;

		DrawStringToHandle(kOptionBaseX + kShadowOffset, kOptionBaseY + i * kOptionIntervalY + kShadowOffset, kOptions[i], 0x000000, fontOptionHandle_);
		DrawStringToHandle(kOptionBaseX, kOptionBaseY + i * kOptionIntervalY, kOptions[i], color, fontOptionHandle_);
	}
	//選択するカーソルを上下に揺らす
	float cursorOffset = sinf(frame_ * kCursorShakeSpeed) * kCursorShakeRange;

	DrawRotaGraph(kCursorX, kCursorBaseY + selectIdx_ * kOptionIntervalY + static_cast<int>(cursorOffset), 
		kCursorScale, kCursorAngle, selectHandle_, true);

	int startX = kScreenCenterX - (static_cast<int>(gameOverText_.size()) * kCharWidth) / 2;

	for (size_t i = 0; i < gameOverText_.size(); ++i)
	{
		if (!charVisible_[i]) continue;

		float shake = sinf(frame_ * kShakeSpeed + i * kShakePhase) * kShakeRange;

		char s[2] = { gameOverText_[i], '\0' };

		DrawStringToHandle(
			startX + static_cast<int>(i) * kCharWidth + kShadowOffset,
			kTextY + kShadowOffset + static_cast<int>(shake),
			s,
			0x000000,
			fontHandle_);

		DrawStringToHandle(
			startX + static_cast<int>(i) * kCharWidth,
			kTextY + static_cast<int>(shake),
			s,
			0xffff00,
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

	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", kFontSize, -1, -1);
	fontOptionHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", kFontOptionSize, -1, -1);

	bgHandle_ = LoadGraph("data/map/bg.png");
	frameHandle_ = LoadGraph("data/UI/Tutorialframe.png");
	selectHandle_ = LoadGraph("data/Bullet/Knife.png");
	deadCircleHandle_ = LoadGraph("data/UI/playerDeadCircle.png");
	playerDeadGraphHandle_ = LoadGraph("data/Player/Dead.png");
	pAnimation_ = std::make_shared<Animation>(
		playerDeadGraphHandle_,
		kGraphWidth, kGraphHeight, kFrameCount, kFrameInterval, kScale, false, 0);

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