#pragma once
#include "Scene.h"
#include "Camera.h"
#include"Geometry.h"
#include"BulletManager.h"
#include"EnemyFactory.h"
#include<memory>

enum class StageType
{
	Stage1,
	Stage2,
	BossDebugStage
};

class Player;
class Bg;
class GameScene :public Scene
{
private:

	int frame_ = 0;// フェードインアウト用

	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	void GoalFadeOutUpdate(Input&);
	using UpdateFunc_t = void (GameScene::*)(Input&);
	UpdateFunc_t update_;//update系を受け取るメンバ関数ポインタ

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (GameScene::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ

public:
	GameScene(SceneController& controller,StageType stage);
	void Init();
	void Update(Input&)override;
	void Draw()override;

	StageType GetNextStageType(StageType nextStage);

private:
	std::shared_ptr<Player>pPlayer_;
	BulletManager bulletManager_;
	EnemyFactory enemyFactory_;
	std::shared_ptr<Bg>bg_;
	std::shared_ptr<Camera>pCamera_;


	//ボスを倒した後に落ちる鍵関連
	int keyH_;//鍵のハンドル
	Rect keyRect_;//鍵の当たり判定
	Vector2 keyPos_;//鍵の座標
	StageType stageType_;
	bool isKeyActive_;//鍵が出現しているかどうか

};