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
		700.0f,   //Jump
		1100.0f,  //DoubleJump
		1500.0f,  //Attack
		1900.0f   //WeaponChange
	};

	const char* kTutorialText[] =
	{
		"Move with Arrow Keys",
		"Press Jump Button",
		"Try Double Jump",
		"Attack!",
		"Change Weapon"
	};
}

void TutorialManager::Init(GameProgress* progress)
{
	gameProgress_ = progress;
	isTutorialFinished_ = false;
	currentStep_ = TutorialStep::Move;

	isWaitingAction_ = false;
	waitingMessage_ = nullptr;
}

void TutorialManager::Update(Player& player, Input& input)
{
	if (isTutorialFinished_)return;
	if (!gameProgress_) return;

	if (!isWaitingAction_)
	{
		int stepIndex = (int)currentStep_;

		if (currentStep_ != TutorialStep::Finish &&
			player.GetPos().x >= kTutorialX[stepIndex])
		{
			isWaitingAction_ = true;
			isWaitingConfirm_ = true;
			waitingMessage_ = kTutorialText[stepIndex];
			//移動以外のチュートリアル中は操作不可にする
			if (currentStep_ != TutorialStep::Move)
			{
				player.SetControllable(false);
			}
		}
	}

	if (isWaitingConfirm_)
	{
		if (input.IsTriggered("next"))
		{
			isWaitingConfirm_ = false;
			player.SetControllable(true);
		}
		return;
	}

	CheckTutorialStep(player);
}

void TutorialManager::Draw() const
{
	if (!waitingMessage_) return;

	//背景
	DrawBox(300, 400, 980, 520, GetColor(0, 0, 0), TRUE);

	//テキスト
	DrawString(340, 440, waitingMessage_, GetColor(255, 255, 255));
}

bool TutorialManager::IsTutorialFinished() const
{
	return isTutorialFinished_;
}

void TutorialManager::CheckTutorialStep(Player& player)
{
	if (!isWaitingAction_ || isWaitingConfirm_) return;

	switch (currentStep_)
	{
	case TutorialStep::Move:
		if (gameProgress_->tutorialMoved_)
		{
			currentStep_ = TutorialStep::Jump;
			isWaitingAction_ = false;
			waitingMessage_ = nullptr;
			player.SetControllable(true);
		}
		break;

	case TutorialStep::Jump:
		if (gameProgress_->tutorialJumped_)
		{
			currentStep_ = TutorialStep::DoubleJump;
			isWaitingAction_ = false;
			waitingMessage_ = nullptr;
			player.SetControllable(true);
		}
		break;

	case TutorialStep::DoubleJump:
		if (gameProgress_->tutorialDoubleJumped_)
		{
			currentStep_ = TutorialStep::Attack;
			isWaitingAction_ = false;
			waitingMessage_ = nullptr;
			player.SetControllable(true);
		}
		break;

	case TutorialStep::Attack:
		if (gameProgress_->tutorialAttacked_)
		{
			currentStep_ = TutorialStep::WeaponChange;
			isWaitingAction_ = false;
			waitingMessage_ = nullptr;
			player.SetControllable(true);
		}
		break;

	case TutorialStep::WeaponChange:
		if (gameProgress_->tutorialWeaponChanged_)
		{
			currentStep_ = TutorialStep::Finish;
			isTutorialFinished_ = true;

			isWaitingAction_ = false;
			waitingMessage_ = nullptr;
			player.SetControllable(true);
		}
		break;

	default:
		break;
	}
}
