#pragma once
class GameProgress
{
private:
	//ゲームからのタイトルに戻って来たかのフラグ
	bool isReturnFromGame_ = false;

public:
	bool isDefeatedBoss1_ = false;//ステージ1のボスを倒したかどうか
	int deathCount_ = 0;//プレイヤーが死んだ回数
	int playCount_ = 0;//プレイした回数
	//チュートリアル進捗
	int tutorialStep_ = 0;

	//ゲームからのタイトルに戻って来たかどうかの取得・設定関数
	bool IsReturnFromGame() const { return isReturnFromGame_; }
	void SetReturnFromGame(bool flag) { isReturnFromGame_ = flag; }
	void Reset() { isDefeatedBoss1_ = false; deathCount_ = 0; }
};

