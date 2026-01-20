#pragma once
#include <map>

enum class SE
{
	Decide,
	Cancel,
	Select,
	Shot,
	Wave,
	Hit,
	Damage,
	BossGuard,
	BossGuardBreak,
	BossDeath,
	Explosion,
	WeaponChange,
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
	int volume_ = 255;
};

