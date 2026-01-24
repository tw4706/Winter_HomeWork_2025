#pragma once
#include "Scene.h"
#include"Input.h"
#include"StageType.h"
#include<memory>

//タイトルシーンの状態
enum class TitleState
{
	Normal,//通常
	Demo,
	Confirm//初回プレイか確認中
};

class Bg;
class DemoPlayer;
class DemoZombie;
class TitleScene :public Scene
{
private:
	int titleH_;//タイトル画像ハンドル
	int pressStartH_;//PressStart画像ハンドル
	int selectH_;//選択画像ハンドル
	int fontH_;//フォントハンドル
	int bgH_;//背景のハンドル
	int frame_;//経過フレーム
	int pressBlinkFrame_;//PressStart点滅用フレーム
	int titleRogoPulseFrame_;//タイトルロゴが揺れるフレーム
	bool isDeciding_;//決定中かどうか
	int decideBlinkCount_;//決定時の点滅カウント
	bool isSkipedConfirm_;//確認をスキップしたかどうか
	StageType nextStage_;//次のステージ
	std::shared_ptr<DemoPlayer> demoPlayer_;//デモプレイヤー
	std::vector<std::shared_ptr<DemoZombie>> demoZombies_;
	std::shared_ptr<Bg>pBg_;

	TitleState titleState_;//シーンの状態
	int confirmSelect_;//確認選択肢 0 = はい 1 = いいえ

private:
	void FadeInUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);
	void ConfirmUpdate(Input& input);
	void PlayerRunUpdate(Input&);
	using UpdateFunc_t = void (TitleScene::*)(Input&);
	UpdateFunc_t update_;

	void NormalDraw();
	void FadeDraw();
	void ConfirmDraw();
	using DrawFunc_t = void(TitleScene::*)();
	DrawFunc_t draw_;

public:
	TitleScene(SceneController&);
	~TitleScene();

	void Init()override;
	void Update(Input&)override;
	void Draw()override;
};

