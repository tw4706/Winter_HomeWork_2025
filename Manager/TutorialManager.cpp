#include "TutorialManager.h"
#include "Player.h"
#include"Camera.h"
#include"Input.h"
#include"GameProgress.h"
#include<DxLib.h>

namespace
{
	constexpr int kButtonAreaWidth = 84;
	constexpr int kTextPadding = 16;

	//テキストとボタンの間隔
	constexpr int kButtonTextSpacing = 200;

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
	"Aボタンでアタックだ",
	"Bボタンでジャンプ",
	"さらにBボタンでダブルジャンプが可能",
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
		return;
	}

	//テキスト表示中なら決定ボタンで消す
	if (isWaitingAction_ && input.IsTriggered("next"))
	{
		isWaitingAction_ = false;
		waitingMessage_ = nullptr;
		currentStep_++;  //次のステップへ
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

#ifdef _DEBUG
	int left = static_cast<int>(goalRect_.GetLeft() + camera.GetOffset().x);
	int top = static_cast<int>(goalRect_.GetTop() + camera.GetOffset().y);
	int right = static_cast<int>(goalRect_.GetRight() + camera.GetOffset().x);
	int bottom = static_cast<int>(goalRect_.GetBottom() + camera.GetOffset().y);

	DrawBox(left, top, right, bottom, GetColor(255, 0, 0), FALSE); // 赤枠で表示
#endif

	if (!waitingMessage_) return;

	// フレーム表示座標とサイズ
	const int frameX = 300;
	const int frameY = 200;
	const int frameWidth = 680;  // 好きな横幅に変更
	const int frameHeight = 120; // 好きな縦幅に変更

	if (textFrameHandle_ != -1)
	{
		// 画像を指定サイズに拡大/縮小して描画
		DrawExtendGraph(frameX, frameY, frameX + frameWidth, frameY + frameHeight, textFrameHandle_, TRUE);
	}

	// テキストをフレーム中央に配置
	int textWidth = GetDrawStringWidth(waitingMessage_, static_cast<int>(strlen(waitingMessage_)));
	int textHeight = 15; //文字の高さの目安
	int textX = frameX + kButtonAreaWidth;
	int textY = frameY + (frameHeight - textHeight) / 2-10;

	if (buttonAnim_)
	{
		int buttonX = textX + textWidth + 400;
		int buttonY = (frameY + frameHeight / 2)-10;

		buttonAnim_->Draw(static_cast<float>(buttonX), static_cast<float>(buttonY));
	}

	DrawStringToHandle(textX, textY, waitingMessage_, GetColor(255, 255, 255), fontHandle_);
}

bool TutorialManager::IsTutorialFinished() const
{
	return currentStep_ >= kTutorialCount;
}