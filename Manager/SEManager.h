#pragma once
#include <map>

enum class SE
{
	Decide,
	Cancel,
	Select,
	Shot,
	PlayerJump,
	Death,
	Wave,
	Hit,
	Damage,
	Warp,
	Fall,
	UnlockTorch,
	FlowerAttack,
	EnemyJump,
	BossGuard,
	BossGuardBreak,
	BossMove,
	BossCry,
	BossBreath,
	BossDeath,
	Explosion,
	WeaponChange,
	TutorialText
};

class SEManager
{
public:
	void Init();
	void PlaySE(SE se);
	void SetVolume(int volume);
	int GetVolume() const;
	int GetHandle(SE se);

private:
	std::map<SE, int>seHandles_;
	int volume_ = 0;
};

