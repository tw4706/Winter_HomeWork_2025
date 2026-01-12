#pragma once
#include "Geometry.h"

class Player;
class Input;
class TutorialManager
{
public:
	//使わないのでデフォルトコンストラクタのみ
	TutorialManager() = default;

	void Init();
	void Update(Player&player,Input&input);
	void Draw() const;

	//チュートリアルが終了したかどうかを返す
	bool IsTutorialFinished() const;

private:
	//現在のチュートリアルステップ
	int currentStep_ = 0;
	int textFrameHandle_ = -1;

	//チュートリアルの内容で止める地点関連
	bool isWaitingAction_ = false;
	const char* waitingMessage_ = nullptr;
};

