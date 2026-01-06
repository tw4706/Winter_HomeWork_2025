#pragma once
class GameProgress
{
public:
	bool isDefeatedBoss1_=false;//ステージ1のボスを倒したかどうか

	//チュートリアル進捗
	bool tutorialMoved_ = false;
	bool tutorialJumped_ = false;
	bool tutorialDoubleJumped_ = false;
	bool tutorialAttacked_ = false;

	//チュートリアルが完了しているかどうかを返す
	bool IsTutorialCompleted() const
	{
		return tutorialMoved_
			&& tutorialJumped_
			&& tutorialDoubleJumped_
			&& tutorialAttacked_;}
};

