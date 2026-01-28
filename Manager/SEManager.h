#pragma once
#include <map>

enum class SE
{
	Decide,
	Cancel,
	Select,
	Shot,
	PlayerJump,
	PlayerWalk,
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
	~SEManager();

	void Init();
	void PlaySE(SE se);
	void StopSE(SE se);

	void SetVolume(int volume);
	int GetVolume() const;
	int GetHandle(SE se);
	//âπó ÇDxLibópÇÃâπó Ç…ïœä∑Ç∑ÇÈä÷êî
	int CalcDxVolume(SE se) const;

private:
	std::map<SE, int>seHandles_;
	std::map<SE, int>seBaseVolume_;
	int volume_ = 0;
};

