#include "TutorialManager.h"
#include "Player.h"
#include"Camera.h"
#include"Input.h"
#include"GameProgress.h"
#include<DxLib.h>

namespace
{
	// チュートリアル発生地点（X だけで管理する簡易版）
	const float kTutorialX[] =
	{
		300.0f,   //Move
		1900.0f,   //Jump
		2300.0f,  //DoubleJump
		3200.0f,  //Attack
		4000.0f   //WeaponChange
	};

	const char* kTutorialText[] =
	{
	"←→で移動",
	"Bボタンでジャンプ",
	"さらにBボタンでダブルジャンプが可能",
	"Aボタンで攻撃だ",
	"Xボタンで武器変更ができる"
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

	goalRect_.SetLT(4900.0f, 1724.0f, 64.0f, 64.0f);

	int goalHandle = LoadGraph("data/Effect/bullet_effect.png");

	goalAnim_ = std::make_unique<GoalAnimation>(
		goalHandle,
		16, 16,     // フレームサイズ
		6,          // 使用フレーム数
		6,
		5.0f,
		496,
		0,
		true);
}

void TutorialManager::Update(Player& player, Input& input)
{
	if (goalAnim_)
	{
		goalAnim_->Update();
	}

	if (currentStep_ >= kTutorialCount) return;

	//到達地点でテキスト表示
	if (!isWaitingAction_ && player.GetPos().x >= kTutorialX[currentStep_])
	{
		isWaitingAction_ = true;
		waitingMessage_ = kTutorialText[currentStep_];
		return;
	}

	// テキスト表示中なら決定ボタンで消す
	if (isWaitingAction_ && input.IsTriggered("next"))
	{
		isWaitingAction_ = false;
		waitingMessage_ = nullptr;
		currentStep_++;  // 次のステップへ
	}
}

void TutorialManager::Draw(const Camera& camera)
{
	if (goalAnim_)
	{
		float drawX = goalRect_.GetLeft() + camera.GetOffset().x;
		float drawY = goalRect_.GetTop()+ camera.GetOffset().y;

		goalAnim_->Draw(drawX, drawY);
	}

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
	int textWidth = GetDrawStringWidth(waitingMessage_, strlen(waitingMessage_));
	int textHeight = 16; // 文字の高さの目安
	int textX = frameX + (frameWidth - textWidth) / 2;
	int textY = frameY + (frameHeight - textHeight) / 2;

	DrawString(textX, textY, waitingMessage_, GetColor(255, 255, 255));
}

bool TutorialManager::IsTutorialFinished() const
{
	return currentStep_ >= kTutorialCount;
}
