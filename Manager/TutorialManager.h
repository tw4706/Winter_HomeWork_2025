#pragma once
#include "Rect.h"
#include "Geometry.h"
#include "SpriteEffect.h"
#include "SpriteAnimation.h"
#include <memory>

class Player;
class Input;
class SpriteEffect;
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

	//ゲームが一時停止中かどうかを返す
	bool IsGamePaused() const { return isWaitingAction_; }

private:
	int fontHandle_;
	int textButtonHandle_;
	//現在のチュートリアルステップ
	int currentStep_ = 0;
	int textFrameHandle_ = -1;
	Rect goalRect_;
	bool goalReached_ = false;
	//チュートリアルの内容で止める地点関連
	bool isWaitingAction_ = false;
	const char* waitingMessage_ = nullptr;

	std::unique_ptr<SpriteAnimation> goalAnim_;
	std::unique_ptr<SpriteAnimation> buttonAnim_;
};