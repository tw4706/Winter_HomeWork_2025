#include "BGMManager.h"
#include<Dxlib.h>
#include <algorithm>

void BGMManager::Init()
{
	bgmHandles_[BGM::Title] = LoadSoundMem("data/BGM・SE/TitleBGM.mp3");
	bgmHandles_[BGM::Game] = LoadSoundMem("data/BGM・SE/GameBGM.mp3");
	bgmHandles_[BGM::GameOver] = LoadSoundMem("data/BGM・SE/GameOverBGM.mp3");
	bgmHandles_[BGM::GameClear] = LoadSoundMem("data/BGM・SE/GameClearBGM.mp3");

	bgmBaseVolume_[BGM::Title] = 10;
	bgmBaseVolume_[BGM::Game] = 60;
	bgmBaseVolume_[BGM::GameOver] = 100;
	bgmBaseVolume_[BGM::GameClear] = 100;
}

void BGMManager::PlayBGM(BGM bgm)
{
	int handle = bgmHandles_[bgm];

	if (currentHandle_ == handle)return;

	//現在再生中のBGMを停止する
	if(currentHandle_ != -1)
	{
		StopSoundMem(currentHandle_);
	}
	//再生中のBGMを更新する
	currentHandle_ = handle;

	int finalVol =volume_ * bgmBaseVolume_[bgm] / 100 * 255 / 100;

	ChangeVolumeSoundMem(finalVol, currentHandle_);

	//ループ再生でBGMを再生する
	PlaySoundMem(currentHandle_, DX_PLAYTYPE_LOOP);
}

void BGMManager::StopBGM()
{
	//現在再生中のBGMを停止する
	if(currentHandle_!=-1)
	{
		StopSoundMem(currentHandle_);
		currentHandle_ = -1;
	}
}

void BGMManager::SetVolume(int volume)
{
	volume_ = std::clamp(volume, 0, 100);

	// DxLib は 0～255
	int dxVolume = volume_ * 255 / 100;

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
