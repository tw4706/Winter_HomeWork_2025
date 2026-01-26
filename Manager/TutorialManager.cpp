#include "TutorialManager.h"
#include "Player.h"
#include"Camera.h"
#include"Input.h"
#include"Application.h"
#include"GameProgress.h"
#include<DxLib.h>

namespace
{
	// =============================
	// UIレイアウト関連
	// =============================
	constexpr int kFrameX = 300;
	constexpr int kFrameY = 200;
	constexpr int kFrameWidth = 680;
	constexpr int kFrameHeight = 120;

	constexpr int kTextLeftPadding = 24;
	constexpr int kTextOffsetX = 45;
	constexpr int kTextOffsetY = -5;
	constexpr int kTextHeight = 15;

	constexpr int kButtonPaddingRight = 24;
	constexpr int kButtonSize = 48;
	constexpr int kButtonYOffset = 15;
	constexpr int kButtonOffsetX = 15;

	// =============================
	// ゴール演出関連
	// =============================
	constexpr float kGoalPosX = 4800.0f;
	constexpr float kGoalPosY = 1684.0f;
	constexpr float kGoalSize = 64.0f;
	constexpr int   kGoalDrawOffsetY = 50;

	// =============================
	// ゴールアニメーション関連
	// =============================
	constexpr int   kGoalFrameW = 16;
	constexpr int   kGoalFrameH = 16;
	constexpr int   kGoalFrameCount = 6;
	constexpr int   kGoalAnimInterval = 7;
	constexpr float kGoalAnimScale = 7.0f;
	constexpr int   kGoalSrcX = 496;
	constexpr int   kGoalSrcY = 0;

	// =============================
	// ボタンアニメーション関連
	// =============================
	constexpr int   kButtonFrameW = 16;
	constexpr int   kButtonFrameH = 16;
	constexpr int   kButtonFrameCount = 4;
	constexpr int   kButtonAnimInterval = 8;
	constexpr float kButtonAnimScale = 3.0f;
	constexpr int   kButtonSrcW = 64;
	constexpr int   kButtonSrcH = 48;

	// =============================
	// 入力制御関連
	// =============================
	constexpr int kInputLockFrame = 80;

	// =============================
	// フォント関連
	// =============================
	constexpr int kFontSize = 24;
	constexpr int kFontThickness = -1;
	constexpr int kFontType = -1;

	// =============================
	// アニメーション関連
	// =============================
	constexpr int kAppearInterval = 12;
	constexpr int kSlideDistance = 30;
	constexpr int kMaxAlpha = 255;

	//チュートリアルテキストが発生する地点
	const float kTutorialX[] =
	{
		300.0f,		//Move
		1300.0f,	//Attack
		2000.0f,	//Jump
		2000.0f,	//DoubleJump
		3600.0f,	//WeaponChange
		4500.0f
	};

	const char* kTutorialText[] =
	{
	"←→で移動",
	"Xボタンでアタックだ",
	"Aボタンでジャンプ",
	"さらにAボタンでダブルジャンプが可能",
	"Yボタンで武器チェンジが可能",
	"ここからしにがち村に行けそうだ"
	};

	//チュートリアル内容の総数
	constexpr int kTutorialCount = sizeof(kTutorialX) / sizeof(kTutorialX[0]);
}

void TutorialManager::Init()
{
	currentStep_ = 0;
	isWaitingAction_ = false;
	waitingMessage_ = nullptr;
	appearFrame_ = 0;
	isDisappearing_ = false;

	textFrameHandle_ = LoadGraph("data/UI/TutorialFrame.png");
	textButtonHandle_ = LoadGraph("data/UI/gdb-switch-2.png");

	goalRect_.SetLT(kGoalPosX,kGoalPosY,kGoalSize,kGoalSize);

	int goalHandle = LoadGraph("data/Effect/bullet_effect.png");

	goalAnim_ = std::make_unique<SpriteAnimation>(
		goalHandle,
		kGoalFrameW,kGoalFrameH,
		kGoalFrameCount,kGoalAnimInterval,
		kGoalAnimScale,kGoalSrcX,kGoalSrcY,true);

	buttonAnim_ = std::make_unique<SpriteAnimation>(
		textButtonHandle_,
		kButtonFrameW,kButtonFrameH,
		kButtonFrameCount,kButtonAnimInterval,
		kButtonAnimScale,kButtonSrcW,kButtonSrcH,true);
	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢",kFontSize,kFontThickness,kFontType);
}

void TutorialManager::Update(Player& player, Input& input)
{
	if (goalAnim_)
	{
		goalAnim_->Update();
	}

	if (buttonAnim_)
	{
		buttonAnim_->Update();
	}

	if (currentStep_ >= kTutorialCount) return;

	//到達地点でテキスト表示
	if (!isWaitingAction_ && player.GetPos().x >= kTutorialX[currentStep_])
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::TutorialText);
		isWaitingAction_ = true;
		waitingMessage_ = kTutorialText[currentStep_];

		appearFrame_ = 0;
		return;
	}

	//テキストとフレームの表示
	if (isWaitingAction_ && !isDisappearing_)
	{
		inputLockFrame_++;
		if (appearFrame_ < kAppearInterval)
		{
			appearFrame_++;
		}

		if (inputLockFrame_ > kInputLockFrame)
		{
			//決定ボタンでフェードアウト開始
			if (input.IsTriggered("next"))
			{
				isDisappearing_ = true;
				inputLockFrame_ = 0;
				Application::GetInstance().GetSEManager().PlaySE(SE::Select);
			}
		}
	}

	//テキストとフレームのフェードアウト
	if (isWaitingAction_ && isDisappearing_)
	{
		if (appearFrame_ > 0)
		{
			appearFrame_--;
		}
		else
		{
			//完全に消えたら次へ
			isWaitingAction_ = false;
			isDisappearing_ = false;
			waitingMessage_ = nullptr;
			currentStep_++;
		}
	}
}

void TutorialManager::Draw(const Camera& camera)
{
	if (goalAnim_)
	{
		float drawX = goalRect_.GetLeft() + camera.GetOffset().x;
		float drawY = goalRect_.GetTop() + camera.GetOffset().y + kGoalDrawOffsetY;

		goalAnim_->Draw(drawX, drawY);
	}

	if (!waitingMessage_) return;

	float rate =static_cast<float>(appearFrame_) / kAppearInterval;
	if (rate > 1.0f) rate = 1.0f;

	rate *= rate;

	int alpha = static_cast<int>(kMaxAlpha * rate);
	int slideY =static_cast<int>((1.0f - rate) * kSlideDistance);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	// ===== フレーム =====
	DrawExtendGraph(kFrameX,kFrameY + slideY,
		kFrameX + kFrameWidth,kFrameY + kFrameHeight + slideY,
		textFrameHandle_,TRUE);

	// ===== テキスト =====
	int textX =kFrameX + kTextLeftPadding + kTextOffsetX;
	int textY =kFrameY +(kFrameHeight - kTextHeight) / 2 +kTextOffsetY + slideY;

	DrawStringToHandle(textX,textY,
		waitingMessage_,GetColor(255, 255, 255),fontHandle_);

	// ===== ボタン =====
	if (buttonAnim_)
	{
		int buttonX =kFrameX + kFrameWidth -kButtonPaddingRight -kButtonSize -kButtonOffsetX;

		int buttonY =kFrameY +(kFrameHeight - kButtonSize) / 2 +kButtonYOffset + slideY;

		buttonAnim_->Draw(static_cast<float>(buttonX),static_cast<float>(buttonY));
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

bool TutorialManager::IsTutorialFinished() const
{
	return currentStep_ >= kTutorialCount;
}