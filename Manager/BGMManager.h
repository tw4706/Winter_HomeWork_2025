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
	~BGMManager();

	void Init();
	void PlayBGM(BGM bgm);
	void StopBGM();

	void SetVolume(int volume);
	int GetVolume() const;

private:
	std::map<BGM, int>bgmHandles_;
	std::map<BGM, int> bgmBaseVolume_;//BGM‚²‚Æ‚ÌŠî–{‚Ì‰¹—Ê

	int currentHandle_ = -1;
	int volume_ = 100;
};

