#pragma once
#include <map>

enum class SE
{
	Decide,
	Cancel,
	Select,
	Shot,
	Hit,
	Damage,
	WeaponChange,
};

class SEManager
{
public:
	void Init();
	void PlaySE(SE se);
	void SetVolume(int volume);

private:
	std::map<SE, int>seHandles_;
	int volume_ = 200;
};

