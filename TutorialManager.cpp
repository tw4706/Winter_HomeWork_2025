#include "TutorialManager.h"

void TutorialManager::Init()
{
	isTutorialFinished_ = false;
}

void TutorialManager::Update(const Player& player, const EnemyFactory& ef)
{
}

bool TutorialManager::IsTutorialFinished() const
{
	return isTutorialFinished_;
}
