#include "BGMManager.h"
#include<Dxlib.h>
#include <algorithm>

namespace
{
	// ==============================
	// 音量関連
	// ==============================
	constexpr int kVolumeMin = 0;
	constexpr int kVolumeMax = 100;
	constexpr int kDxVolumeMax = 255;
	constexpr int kBgmBaseVolume = 50;

	// ==============================
	// ハンドル関連
	// ==============================
	constexpr int kInvalidHandle = -1;

	// ==============================
	// 再生設定関連
	// ==============================
	constexpr int kBgmPlayType = DX_PLAYTYPE_LOOP;

	// ==============================
	// BGM パス関連
	// ==============================
	constexpr char kTitleBgmPath[] = "data/BGM・SE/TitleBGM.mp3";
	constexpr char kGameBgmPath[] = "data/BGM・SE/GameBGM.mp3";
	constexpr char kGameOverBgmPath[] = "data/BGM・SE/GameOverBGM.mp3";
	constexpr char kGameClearBgmPath[] = "data/BGM・SE/夏の思い出.mp3";
}

void BGMManager::Init()
{
	bgmHandles_[BGM::Title] = LoadSoundMem(kTitleBgmPath);
	bgmHandles_[BGM::Game] = LoadSoundMem(kGameBgmPath);
	bgmHandles_[BGM::GameOver] = LoadSoundMem(kGameOverBgmPath);
	bgmHandles_[BGM::GameClear] = LoadSoundMem(kGameClearBgmPath);

	bgmBaseVolume_[BGM::Title] = kBgmBaseVolume;
	bgmBaseVolume_[BGM::Game] = kBgmBaseVolume;
	bgmBaseVolume_[BGM::GameOver] = kBgmBaseVolume;
	bgmBaseVolume_[BGM::GameClear] = kBgmBaseVolume;
}

void BGMManager::PlayBGM(BGM bgm)
{
	int handle = bgmHandles_[bgm];

	if (currentHandle_ == handle)return;

	//現在再生中のBGMを停止する
	if(currentHandle_ != kInvalidHandle)
	{
		StopSoundMem(currentHandle_);
	}
	//再生中のBGMを更新する
	currentHandle_ = handle;

	//実際の音量(0～100で表示されるやつ)
	int logicalVol = volume_ * bgmBaseVolume_[bgm] / kVolumeMax;
	//こっちはDXLibが変換しているやつ
	int vol = logicalVol * kDxVolumeMax / kVolumeMax;
	ChangeVolumeSoundMem(vol, currentHandle_);

	PlaySoundMem(currentHandle_, kBgmPlayType);
}

void BGMManager::StopBGM()
{
	//現在再生中のBGMを停止する
	if(currentHandle_!= kInvalidHandle)
	{
		StopSoundMem(currentHandle_);
		currentHandle_ = kInvalidHandle;
	}
}

void BGMManager::SetVolume(int volume)
{
	volume_ = std::clamp(volume, kVolumeMin, kVolumeMax);

	int dxVolume = volume_ * kDxVolumeMax / kVolumeMax;

	// 全BGMに反映（重要）
	for (auto& [bgm, handle] : bgmHandles_)
	{
		ChangeVolumeSoundMem(dxVolume, handle);
	}
}

int BGMManager::GetVolume() const
{
	return volume_;
}
