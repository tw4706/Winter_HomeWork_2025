#include "SEManager.h"
#include<DxLib.h>
#include <algorithm>

void SEManager::Init()
{
	volume_ = 60;

	seHandles_[SE::Decide] = LoadSoundMem("data/BGM・SE/decision-3.mp3");
	seHandles_[SE::Cancel] = LoadSoundMem("data/BGM・SE/cancel.mp3");
	seHandles_[SE::Select] = LoadSoundMem("data/BGM・SE/cursorMove.mp3");
	seHandles_[SE::Shot] = LoadSoundMem("data/BGM・SE/shot.wav");
	seHandles_[SE::PlayerJump] = LoadSoundMem("data/BGM・SE/playerJump.wav");
	seHandles_[SE::Death] = LoadSoundMem("data/BGM・SE/death.mp3");
	seHandles_[SE::Wave] = LoadSoundMem("data/BGM・SE/wave.mp3");
	seHandles_[SE::Hit] = LoadSoundMem("data/BGM・SE/enemyhit.mp3");
	seHandles_[SE::Damage] = LoadSoundMem("data/BGM・SE/damage.mp3");
	seHandles_[SE::Warp] = LoadSoundMem("data/BGM・SE/tutorialWarp.wav");
	seHandles_[SE::Fall] = LoadSoundMem("data/BGM・SE/fall.wav");
	seHandles_[SE::UnlockTorch] = LoadSoundMem("data/BGM・SE/unlockTorch.wav");
	seHandles_[SE::FlowerAttack] = LoadSoundMem("data/BGM・SE/flowerAttack.wav");
	seHandles_[SE::EnemyJump] = LoadSoundMem("data/BGM・SE/enemyJump.wav");
	seHandles_[SE::BossGuard] = LoadSoundMem("data/BGM・SE/guard.mp3");
	seHandles_[SE::BossGuardBreak] = LoadSoundMem("data/BGM・SE/guardBreak.mp3");
	seHandles_[SE::BossMove] = LoadSoundMem("data/BGM・SE/boss1Move.mp3");
	seHandles_[SE::BossCry] = LoadSoundMem("data/BGM・SE/bossNakigoe.wav");
	seHandles_[SE::BossBreath] = LoadSoundMem("data/BGM・SE/bossBureath.wav");
	seHandles_[SE::BossDeath] = LoadSoundMem("data/BGM・SE/bigExplosion.mp3");
	seHandles_[SE::Explosion] = LoadSoundMem("data/BGM・SE/battery1.mp3");
	seHandles_[SE::WeaponChange] = LoadSoundMem("data/BGM・SE/weaponChange.mp3");
	seHandles_[SE::TutorialText] = LoadSoundMem("data/BGM・SE/tutorialText.mp3");
}

void SEManager::PlaySE(SE se)
{
	int handle = seHandles_[se];

	int vol = volume_ * 255 / 100;
	ChangeVolumeSoundMem(vol, handle);

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

int SEManager::GetHandle(SE se)
{
	return seHandles_[se];
}
