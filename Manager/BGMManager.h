#pragma once
#include <map>

enum class BGM
{
	Title,
	Game,
	GameOver,
	GameClear
};

class BGMManager
{
public:
	void Init();
	void PlayBGM(BGM bgm);
	void StopBGM();

private:
	std::map<BGM, int>bgmHandles_;
	int currentHandle_ = -1;
};

