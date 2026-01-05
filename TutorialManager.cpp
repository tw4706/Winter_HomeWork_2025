#include "TutorialManager.h"
#include "Player.h"
#include"Camera.h"
#include"GameProgress.h"
#include<DxLib.h>

void TutorialManager::Init(GameProgress* progress)
{
	gameProgress_ = progress;
	isTutorialFinished_ = false;
	currentStep_ = TutorialStep::Move;
}

void TutorialManager::Update(const Player& player, const EnemyFactory& ef)
{
	if (isTutorialFinished_)return;
	if (!gameProgress_) return;

	CheckTutorialStep(player, ef);
}

void TutorialManager::Draw() const
{
	const int baseX = 40;    // âÊñ ç∂
	const int baseY = 80;    // âÊñ è„
	const int lineHeight = 28;

	const char* texts[] =
	{
		"Move",
		"Jump",
		"Double Jump",
		"Attack",
		"Get Hit"
	};

	for (int i = 0; i < (int)TutorialStep::Finish; ++i)
	{
		int drawX = baseX;
		int drawY = baseY + i * lineHeight;

		DrawString(drawX, drawY,
			stepCompleted_[i] ? "ÅZ" : "Å~",
			GetColor(255, 255, 255));

		DrawString(drawX + 24, drawY,
			texts[i],
			GetColor(255, 255, 255));
	}
}

bool TutorialManager::IsTutorialFinished() const
{
	return isTutorialFinished_;
}

void TutorialManager::CheckTutorialStep(const Player& player, const EnemyFactory& ef)
{
	if (!gameProgress_) return;

	switch (currentStep_)
	{
	case TutorialStep::Move:
		if (gameProgress_->tutorialMoved_)
		{
			stepCompleted_[(int)TutorialStep::Move] = true;
			currentStep_ = TutorialStep::Jump;
		}
		break;

	case TutorialStep::Jump:
		if (gameProgress_->tutorialJumped_)
		{
			stepCompleted_[(int)TutorialStep::Jump] = true;
			currentStep_ = TutorialStep::DoubleJump;
		}
		break;

	case TutorialStep::DoubleJump:
		if (gameProgress_->tutorialDoubleJumped_)
		{
			stepCompleted_[(int)TutorialStep::DoubleJump] = true;
			currentStep_ = TutorialStep::Attack;
		}
		break;

	case TutorialStep::Attack:
		if (gameProgress_->tutorialAttacked_)
		{
			stepCompleted_[(int)TutorialStep::Attack] = true;
			currentStep_ = TutorialStep::Damaged;
		}
		break;

	case TutorialStep::Damaged:
		if (gameProgress_->tutorialDamaged_)
		{
			stepCompleted_[(int)TutorialStep::Damaged] = true;
			currentStep_ = TutorialStep::Finish;
			isTutorialFinished_ = true;
		}
		break;
	default:
		break;
	}
}
