#include "SEManager.h"
#include<DxLib.h>
#include <algorithm>

void SEManager::Init()
{
	seHandles_[SE::Decide] = LoadSoundMem("data/BGM・SE/decide.wav");
	seHandles_[SE::Cancel] = LoadSoundMem("data/BGM・SE/cancel.wav");
	seHandles_[SE::Select] = LoadSoundMem("data/BGM・SE/select.wav");
	seHandles_[SE::Shot] = LoadSoundMem("data/BGM・SE/Shot.wav");
	seHandles_[SE::Hit] = LoadSoundMem("data/BGM・SE/Hit.wav");
	seHandles_[SE::Damage] = LoadSoundMem("data/BGM・SE/Damage.wav");
	seHandles_[SE::WeaponChange] = LoadSoundMem("data/BGM・SE/WeaponChange.wav");
}

void SEManager::PlaySE(SE se)
{
	int handle = seHandles_[se];

	ChangeVolumeSoundMem(volume_, handle);

	PlaySoundMem(handle, DX_PLAYTYPE_BACK);
}

void SEManager::SetVolume(int volume)
{
	volume_ = std::clamp(volume, 0, 100);

	int dxVol = volume_ * 255 / 100;
	for (auto& [se, handle] : seHandles_)
	{
		ChangeVolumeSoundMem(dxVol, handle);
	}
}

int SEManager::GetVolume() const
{
	return volume_;
}
