#pragma once
#include "Geometry.h"

class Player;
class EnemyFactory;
class GameProgress;
class TutorialManager
{
public:
	//使わないのでデフォルトコンストラクタのみ
	TutorialManager() = default;

	void Init(GameProgress* progress);
	void Update(Player&player);
	void Draw() const;

	//チュートリアルが終了したかどうかを返す
	bool IsTutorialFinished() const;

private:
	enum class TutorialStep
	{
		Move,
		Jump,
		DoubleJump,
		Attack,
		WeaponChange,
		Finish
	};

	//各チュートリアルステップの進捗具合
	void CheckTutorialStep(Player&player);

private:
	//ゲーム進行状況のポインタ
	GameProgress* gameProgress_;

	//チュートリアルが終了したかどうか
	bool isTutorialFinished_=false;

	//現在のチュートリアルステップ
	TutorialStep currentStep_=TutorialStep::Move;

	//チュートリアルの内容で止める地点関連
	bool isWaitingAction_ = false;
	const char* waitingMessage_ = nullptr;
};

