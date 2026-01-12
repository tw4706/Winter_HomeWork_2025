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
		1200.0f,  //Attack
		4000.0f   //WeaponChange
	};

	const char* kTutorialText[] =
	{
		"←→で移動",
		"Bボタンでジャンプ",
		"さらにBボタンでダブルジャンプが可能!",
		"Aボタンで攻撃だ！",
		""
	};

	//チュートリアル内容の総数
	constexpr int kTutorialCount = sizeof(kTutorialX) / sizeof(kTutorialX[0]);
}

void TutorialManager::Init()
{
	currentStep_ = 0;
	isWaitingAction_ = false;
	waitingMessage_ = nullptr;
}

void TutorialManager::Update(Player& player, Input& input)
{
	if (currentStep_ >= kTutorialCount) return;

	// 到達地点でテキスト表示
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

void TutorialManager::Draw() const
{
	if (!waitingMessage_) return;

	//背景
	DrawBox(300, 200, 980, 320, GetColor(0, 0, 0), TRUE);

	//テキスト
	DrawString(340, 240, waitingMessage_, GetColor(255, 255, 255));
}

bool TutorialManager::IsTutorialFinished() const
{
	return currentStep_ >= kTutorialCount;
}
