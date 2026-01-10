#pragma once
#include "Scene.h"
#include"Input.h"
#include"StageType.h"
#include"BulletManager.h"
#include<memory>

//タイトルシーンの状態
enum class TitleState
{
	Normal,//通常
	Demo,
	Confirm,//初回プレイか確認中
	Exit
};

enum class ExitPhase
{
	Walk,
	Fade
};

class Player;
class Zombie;
class TitleScene :public Scene
{
private:
	int titleH_;//タイトル画像ハンドル
	int pressStartH_;//PressStart画像ハンドル
	int selectH_;//選択画像ハンドル
	int frame_;//経過フレーム
	int pressBlinkFrame_;//PressStart点滅用フレーム
	bool isDeciding_;//決定中かどうか
	int decideBlinkCount_;//決定時の点滅カウント

	TitleState titleState_;//シーンの状態
	int confirmSelect_;//確認選択肢 0 = はい 1 = いいえ
	std::shared_ptr<Player> demoPlayer_;
	std::shared_ptr<Zombie> demoZombie_;
	ExitPhase exitPhase_;
	StageType nextStage_;
	Input dummyInput_;
	BulletManager demoBulletManager_;


	int demoFrame_;

private:
	void FadeInUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);
	void DemoUpdate(Input&);
	void ConfirmUpdate(Input& input);
	void ExitUpdate(Input&);
	using UpdateFunc_t = void (TitleScene::*)(Input&);
	UpdateFunc_t update_;

	void NormalDraw();
	void FadeDraw();
	void DemoDraw();
	void ConfirmDraw();
	void ExitDraw();
	using DrawFunc_t = void(TitleScene::*)();
	DrawFunc_t draw_;


public:
	TitleScene(SceneController&);
	~TitleScene();

	void Init()override;
	void Update(Input&)override;
	void Draw()override;
};

