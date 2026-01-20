#include "TutorialManager.h"
#include "Player.h"
#include"Camera.h"
#include"Input.h"
#include"GameProgress.h"
#include<DxLib.h>

namespace
{
	// =============================
	// UIレイアウト系
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
	// アニメーション
	// =============================
	constexpr int kAppearInterval = 12;
	constexpr int kSlideDistance = 30;
	constexpr int kMaxAlpha = 255;

	// チュートリアル発生地点（X だけで管理する簡易版）
	const float kTutorialX[] =
	{
		300.0f,   //Move
		1300.0f,   //Attack
		2000.0f,  //Jump
		2000.0f,  //DoubleJump
		3800.0f   //WeaponChange
	};

	const char* kTutorialText[] =
	{
	"←→で移動",
	"Bボタンでアタックだ",
	"Aボタンでジャンプ",
	"さらにAボタンでダブルジャンプが可能",
	"Xボタンで武器チェンジが可能"
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

	goalRect_.SetLT(4800.0f, 1684.0f, 64.0f, 64.0f);

	int goalHandle = LoadGraph("data/Effect/bullet_effect.png");

	goalAnim_ = std::make_unique<SpriteAnimation>(
		goalHandle,
		16, 16,     // フレームサイズ
		6,          // 使用フレーム数
		7,
		7.0f,
		496,
		0,
		true);

	buttonAnim_ = std::make_unique<SpriteAnimation>(
		textButtonHandle_,
		16, 16,   // 1フレームサイズ
		4,        // フレーム数
		8,        // 切り替え間隔
		3.0f,
		64,48,
		true);
	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 24, -1, -1);
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
		isWaitingAction_ = true;
		waitingMessage_ = kTutorialText[currentStep_];

		appearFrame_ = 0;
		return;
	}

	//テキストとフレームの表示
	if (isWaitingAction_ && !isDisappearing_)
	{
		if (appearFrame_ < kAppearInterval)
		{
			appearFrame_++;
		}

		//決定ボタンでフェードアウト開始
		if (input.IsTriggered("next"))
		{
			isDisappearing_ = true;
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
		float drawY = goalRect_.GetTop() + camera.GetOffset().y + 50;

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
	DrawExtendGraph(
		kFrameX,
		kFrameY + slideY,
		kFrameX + kFrameWidth,
		kFrameY + kFrameHeight + slideY,
		textFrameHandle_,
		TRUE);

	// ===== テキスト =====
	int textX =kFrameX + kTextLeftPadding + kTextOffsetX;
	int textY =kFrameY +(kFrameHeight - kTextHeight) / 2 +kTextOffsetY + slideY;

	DrawStringToHandle(
		textX,
		textY,
		waitingMessage_,
		GetColor(255, 255, 255),
		fontHandle_);

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