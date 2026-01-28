#include "SEManager.h"
#include<DxLib.h>
#include <algorithm>

namespace
{
	// ==============================
	// 音量関連関連
	// ==============================
	constexpr int kDefaultSEVolume = 80;
	constexpr int kPlayerJumpSEVolume = 100;
	constexpr int kVolumeMin = 0;
	constexpr int kVolumeMax = 100;
	constexpr int kDxVolumeMax = 255;

	constexpr int kBaseVolumeDefault = 100;

	// ==============================
	// 再生設定関連
	// ==============================
	constexpr int kSEPlayType = DX_PLAYTYPE_BACK;

	// ==============================
	// SE パス関連
	// ==============================
	constexpr char kDecideSEPath[] = "data/BGM・SE/decision-3.mp3";
	constexpr char kCancelSEPath[] = "data/BGM・SE/cancel.mp3";
	constexpr char kSelectSEPath[] = "data/BGM・SE/cursorMove.mp3";
	constexpr char kShotSEPath[] = "data/BGM・SE/shot.wav";
	constexpr char kPlayerJumpSEPath[] = "data/BGM・SE/playerJump.mp3";
	constexpr char kPlayerWalkSEPath[] = "data/BGM・SE/playeWalk.mp3";
	constexpr char kDeathSEPath[] = "data/BGM・SE/death.mp3";
	constexpr char kWaveSEPath[] = "data/BGM・SE/wave.mp3";
	constexpr char kHitSEPath[] = "data/BGM・SE/enemyhit.mp3";
	constexpr char kDamageSEPath[] = "data/BGM・SE/damage.mp3";
	constexpr char kWarpSEPath[] = "data/BGM・SE/tutorialWarp.wav";
	constexpr char kFallSEPath[] = "data/BGM・SE/fall.wav";
	constexpr char kUnlockTorchSEPath[] = "data/BGM・SE/unlockTorch.wav";
	constexpr char kFlowerAttackSEPath[] = "data/BGM・SE/flowerAttack.wav";
	constexpr char kEnemyJumpSEPath[] = "data/BGM・SE/enemyJump.wav";
	constexpr char kBossGuardSEPath[] = "data/BGM・SE/guard.mp3";
	constexpr char kBossGuardBreakSEPath[] = "data/BGM・SE/guardBreak.mp3";
	constexpr char kBossMoveSEPath[] = "data/BGM・SE/boss1Move.mp3";
	constexpr char kBossCrySEPath[] = "data/BGM・SE/bossNakigoe.wav";
	constexpr char kBossBreathSEPath[] = "data/BGM・SE/bossBureath.wav";
	constexpr char kBossDeathSEPath[] = "data/BGM・SE/bigExplosion.mp3";
	constexpr char kExplosionSEPath[] = "data/BGM・SE/battery1.mp3";
	constexpr char kWeaponChangeSEPath[] = "data/BGM・SE/weaponChange.mp3";
	constexpr char kTutorialTextSEPath[] = "data/BGM・SE/tutorialText.mp3";
}

SEManager::~SEManager()
{
	for (auto se : seHandles_)
	{
		DeleteSoundMem(se.second);
	}
}

void SEManager::Init()
{
	volume_ = kDefaultSEVolume;

	seHandles_[SE::Decide]			= LoadSoundMem(kDecideSEPath);
	seHandles_[SE::Cancel]			= LoadSoundMem(kCancelSEPath);
	seHandles_[SE::Select]			= LoadSoundMem(kSelectSEPath);
	seHandles_[SE::Shot]			= LoadSoundMem(kShotSEPath);
	seHandles_[SE::PlayerJump]		= LoadSoundMem(kPlayerJumpSEPath);
	seHandles_[SE::PlayerWalk]		= LoadSoundMem(kPlayerWalkSEPath);
	seHandles_[SE::Death]			= LoadSoundMem(kDeathSEPath);
	seHandles_[SE::Wave]			= LoadSoundMem(kWaveSEPath);
	seHandles_[SE::Hit]				= LoadSoundMem(kHitSEPath);
	seHandles_[SE::Damage]			= LoadSoundMem(kDamageSEPath);
	seHandles_[SE::Warp]			= LoadSoundMem(kWarpSEPath);
	seHandles_[SE::Fall]			= LoadSoundMem(kFallSEPath);
	seHandles_[SE::UnlockTorch]		= LoadSoundMem(kUnlockTorchSEPath);
	seHandles_[SE::FlowerAttack]	= LoadSoundMem(kFlowerAttackSEPath);
	seHandles_[SE::EnemyJump]		= LoadSoundMem(kEnemyJumpSEPath);
	seHandles_[SE::BossGuard]		= LoadSoundMem(kBossGuardSEPath);
	seHandles_[SE::BossGuardBreak]	= LoadSoundMem(kBossGuardBreakSEPath);
	seHandles_[SE::BossMove]		= LoadSoundMem(kBossMoveSEPath);
	seHandles_[SE::BossCry]			= LoadSoundMem(kBossCrySEPath);
	seHandles_[SE::BossBreath]		= LoadSoundMem(kBossBreathSEPath);
	seHandles_[SE::BossDeath]		= LoadSoundMem(kBossDeathSEPath);
	seHandles_[SE::Explosion]		= LoadSoundMem(kExplosionSEPath);
	seHandles_[SE::WeaponChange]	= LoadSoundMem(kWeaponChangeSEPath);
	seHandles_[SE::TutorialText]	= LoadSoundMem(kTutorialTextSEPath);

	seBaseVolume_[SE::PlayerJump]	= kPlayerJumpSEVolume;

	SetVolume(volume_);
}

void SEManager::PlaySE(SE se)
{
	const int handle = seHandles_.at(se);
	ChangeVolumeSoundMem(CalcDxVolume(se), handle);
	PlaySoundMem(handle, kSEPlayType);
}

void SEManager::StopSE(SE se)
{
	const int handle = seHandles_.at(se);
	if (CheckSoundMem(handle))
	{
		StopSoundMem(handle);
	}
}

void SEManager::SetVolume(int volume)
{
	volume_ = std::clamp(volume, kVolumeMin, kVolumeMax);

	for (auto& [se, handle] : seHandles_)
	{
		ChangeVolumeSoundMem(CalcDxVolume(se), handle);
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

//音量の計算
int SEManager::CalcDxVolume(SE se) const
{
	const int base =seBaseVolume_.count(se) ? seBaseVolume_.at(se) : kBaseVolumeDefault;

	const int logical = volume_ * base / kVolumeMax;
	return logical * kDxVolumeMax / kVolumeMax;
}
