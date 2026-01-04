#pragma once

class Player;
class EnemyFactory;
class TutorialManager
{
public:
	//使わないのでデフォルトコンストラクタのみ
	TutorialManager() = default;

	void Init();
	void Update(const Player&player,
		const EnemyFactory&ef);

	//チュートリアルが終了したかどうかを返す
	bool IsTutorialFinished() const;

private:
	bool isTutorialFinished_;
};

