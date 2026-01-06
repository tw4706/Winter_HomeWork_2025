#pragma once
#include "Geometry.h"

class Player;
class Camera;
class EnemyFactory;
class GameProgress;
class TutorialManager
{
public:
	//使わないのでデフォルトコンストラクタのみ
	TutorialManager() = default;

	void Init(GameProgress* progress);
	void Update(const Player&player,
		const EnemyFactory&ef);
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
		Finish
	};

	//各チュートリアルステップの進捗具合
	void CheckTutorialStep(const Player&player,
		const EnemyFactory& ef);

	//ゲーム進行状況のポインタ
	GameProgress* gameProgress_;

	//チュートリアルが終了したかどうか
	bool isTutorialFinished_=false;

	//現在のチュートリアルステップ
	TutorialStep currentStep_=TutorialStep::Move;
	//進捗状況表示用
	bool stepCompleted_[(int)TutorialStep::Finish]{};
};

