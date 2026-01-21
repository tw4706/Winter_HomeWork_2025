#pragma once
class GameProgress
{
private:

	//チュートリアル進捗
	bool tutorialMoved_ = false;
	bool tutorialJumped_ = false;
	bool tutorialDoubleJumped_ = false;
	bool tutorialAttacked_ = false;
	bool tutorialWeaponChanged_ = false;

	//ゲームからのタイトルに戻って来たかのフラグ
	bool isReturnFromGame_ = false;

public:
	bool isDefeatedBoss1_ = false;//ステージ1のボスを倒したかどうか
	int deathCount_;//プレイヤーが死んだ回数
	int playCount_;//プレイした回数
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

	//ゲームからのタイトルに戻って来たかどうかの取得・設定関数
	bool IsReturnFromGame() const { return isReturnFromGame_; }
	void SetReturnFromGame(bool flag) { isReturnFromGame_ = flag; }
	void Reset() { isDefeatedBoss1_ = false; deathCount_ = 0; }
};

