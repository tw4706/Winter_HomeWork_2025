#pragma once
#include "Scene.h"
#include "Camera.h"
#include"Geometry.h"
#include"StageType.h"
#include "PlayerWeaponUI.h"
#include"BulletManager.h"
#include"EffectManager.h"
#include "TutorialManager.h"
#include"EnemyFactory.h"
#include<memory>
#include <array>

enum class ClearState
{
	None,
	BossCameraShake,
	AutoWalk
};

struct HpUI
{
	bool isBroken = false;
	std::unique_ptr<SpriteAnimation> anim;
};

class Player;
class Bg;
class GameProgress;
class GameScene :public Scene
{
private:
	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	void GoalFadeOutUpdate(Input&);
	using UpdateFunc_t = void (GameScene::*)(Input&);
	UpdateFunc_t update_;//update系を受け取るメンバ関数ポインタ

	void FadeDraw();
	void NormalDraw();
	void DrawHpUI();
	void DrawStageText();
	using DrawFunc_t = void (GameScene::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ

public:
	GameScene(SceneController& controller,StageType stage);
	~GameScene();

	void Init();
	void Update(Input&)override;
	void Draw()override;

	void OnDamagedHpUI();

	StageType GetNextStageType(StageType nextStage);

private:
	int frame_ = 0;							// フェードインアウト用
	int stageTextTimer_;
	bool isTorchUnlockMessageShow_;			//メッセージ表示したか
	bool isTorchMessageActive_;
	int torchMesseageTimer_;
	int hpHandle_;
	int fontHandle_;
	int fontTorchTextHandle_;
	StageType stageType_;
	std::shared_ptr<Bg>bg_;
	ClearState clearState_;
	PlayerWeaponUI weaponUI_;
	EnemyFactory enemyFactory_;
	GameProgress*pGameProgress_;
	BulletManager bulletManager_;
	EffectManager effectManager_;
	std::array<HpUI, 3> hpUIs_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	std::unique_ptr<TutorialManager> tutorialManager_;

	bool isBoss1Defeated_;//ボス1撃破フラグ
	float autoWalkFinishX_;//自動で移動する開始位置
};