#pragma once
#include "Scene.h"
#include "Camera.h"
#include"Geometry.h"
#include"StageType.h"
#include "PlayerWeaponUI.h"
#include"BulletManager.h"
#include"EffectManager.h"
#include"EnemyFactory.h"
#include<memory>

enum class ClearState
{
	None,
	BossCameraShake,
	AutoWalk
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
	EffectManager effectManager_;
	EnemyFactory enemyFactory_;
	std::shared_ptr<Bg>bg_;
	std::shared_ptr<Camera>pCamera_;
	StageType stageType_;
	ClearState clearState_;
	PlayerWeaponUI weaponUI_;

	bool isBossDefeated_;
	int autoWalkStartX_;
};