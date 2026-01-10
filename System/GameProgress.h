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
	bool tutorialWeaponChanged_ = false;

public:

	//チュートリアル進捗状況取得関数
	bool IsMoved() const { return tutorialMoved_; }
	bool IsJumped() const { return tutorialJumped_; }
	bool IsDoubleJumped() const { return tutorialDoubleJumped_; }
	bool IsAttacked() const { return tutorialAttacked_; }
	bool IsWeaponChanged() const { return tutorialWeaponChanged_; }

	bool IsTutorialCompleted() const
	{
		return IsMoved()
			&& IsJumped()
			&& IsDoubleJumped()
			&& IsAttacked()
			&& IsWeaponChanged();
	}
};

