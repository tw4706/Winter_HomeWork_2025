#include "BGMManager.h"
#include<Dxlib.h>

void BGMManager::Init()
{
	bgmHandles_[BGM::Title] = LoadSoundMem("data/BGM・SE/TitleBGM.mp3");
	bgmHandles_[BGM::Game] = LoadSoundMem("data/BGM・SE/GameBGM.mp3");
	bgmHandles_[BGM::GameOver] = LoadSoundMem("data/BGM・SE/GameOverBGM.mp3");
	bgmHandles_[BGM::GameClear] = LoadSoundMem("data/BGM・SE/GameClearBGM.mp3");
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
