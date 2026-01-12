#pragma once
#include "Rect.h"
#include "Geometry.h"
#include "SpriteEffect.h"
#include <memory>

class Player;
class Input;
class TutorialManager
{
public:
	//使わないのでデフォルトコンストラクタのみ
	TutorialManager() = default;

	void Init();
	void Update(Player&player,Input&input);
	void Draw(const Camera& camera);

	//チュートリアルが終了したかどうかを返す
	bool IsTutorialFinished() const;

	//ゴールに到達したかどうかを返す
	bool IsGoalReached() const { return goalReached_; }
	const Rect& GetGoalRect() const { return goalRect_; }

private:
	//現在のチュートリアルステップ
	int currentStep_ = 0;
	int textFrameHandle_ = -1;

	//チュートリアルの内容で止める地点関連
	bool isWaitingAction_ = false;
	const char* waitingMessage_ = nullptr;

	Rect goalRect_;
	bool goalReached_ = false;
};

