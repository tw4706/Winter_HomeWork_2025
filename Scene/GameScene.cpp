#include "GameScene.h"
#include"Bg.h"
#include"Dog.h"
#include"Rect.h"
#include"Input.h"
#include"Player.h"
#include"Zombie.h"
#include"Boss1.h"
#include"Boss2.h"
#include"GameProgress.h"
#include"Application.h"
#include "SkullFlower.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "SelectScene.h"
#include "PauseScene.h"
#include "TutorialManager.h"
#include"SceneController.h"
#include"GlobalConstants.h"
#include "CollisionManager.h"
#include<cmath>
#include<cassert>
#include<Dxlib.h>

namespace 
{
	//プレイヤーのスポーン位置
	constexpr float kPlayerSpawnPosX = 200.0f;
	constexpr float kPlayerSpawnPosY = 1744.0f;

	//ボス1のスポーン位置
	constexpr float kBoss1SpawnPosX = 8800.0f;
	constexpr float kBoss1SpawnPosY = 1600.0f;

	//ボス2のスポーン位置
	constexpr float kBoss2SpawnPosX = 8600.0f;
	constexpr float kBoss2SpawnPosY = 1600.0f;

	//フェードまでの間隔
	constexpr int fade_interval = 60;
	constexpr float kColSize = 32.0f;

	//落下判定となる座標
	constexpr float kFallLimit = 1900.0f;

	//プレイヤーが自動で歩行する向きと距離
	constexpr int kPlayerDir = 1;//右向き
	constexpr float kPlayerAutoWalkX = 500.0f;
}

GameScene::GameScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	clearState_(ClearState::None),
	gameProgress_(nullptr),
	frame_(fade_interval),
	isBoss1Defeated_(false),
	autoWalkStartX_(0.0f)
{
	//bgにステージに対応するマップデータをセット
	bg_ = std::make_shared<Bg>(stageType_);
	pCamera_ = std::make_shared<Camera>();
}

void GameScene::FadeInUpdate(Input&)
{
	pCamera_->Update(pPlayer_);

	if (frame_-- <= 0) 
	{
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
		return;
	}
}

void GameScene::NormalUpdate(Input&input)
{
	//ポーズボタンを押したらポーズシーンに遷移
	if (input.IsTriggered("pause"))
	{
		controller_.PushScene(std::make_shared<PauseScene>(controller_));
		return;
	}

	//チュートリアルでテキスト表示中ならゲーム更新を停止させる
	bool isTutorialPaused = false;
	if (tutorialManager_ && !tutorialManager_->IsTutorialFinished())
	{
		tutorialManager_->Update(*pPlayer_, input);
		if (tutorialManager_->IsGamePaused())
		{
			isTutorialPaused = true;
		}
	}

	// ゲーム更新を停止
	if (isTutorialPaused) return;

	if (pPlayer_->GetPos().y > kFallLimit)
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
	}

	//各クラスの更新
	pCamera_->Update(pPlayer_);

	pPlayer_->Update(input, bulletManager_,stageType_);

	if (tutorialManager_)
	{
		tutorialManager_->Update(*pPlayer_, input);
	}

	//UIの更新
	weaponUI_.Update(*pPlayer_);

	//敵の更新
	enemyFactory_.Update();

	//弾の更新
	bulletManager_.SetEffectManager(&effectManager_);
	bulletManager_.Update(input, enemyFactory_.GetEnemies(), *pPlayer_);

	//エフェクトの更新
	effectManager_.SetCameraOffset(pCamera_->GetOffset());
	effectManager_.Update();

	//プレイヤーの弾 × 敵の当たり判定(ボスのOnHitは専用なので個別で当たり判定をする)
	CollisionManager::PlayerBulletsVsEnemies(
		bulletManager_.GetBullets(),enemyFactory_.GetEnemies());

	auto boss2 = enemyFactory_.GetBoss2();
	if (boss2)
	{
		CollisionManager::PlayerBulletsVsBoss2(
			bulletManager_.GetBullets(),
			*boss2);
	}

	//敵の弾 × プレイヤーの当たり判定
	CollisionManager::EnemyBulletsVsPlayer(bulletManager_.GetBullets(),*pPlayer_);

	//プレイヤーが死亡していて死亡アニメーションが終了している場合はゲームオーバーシーンへ遷移
	if (pPlayer_->IsDead() && pPlayer_->IsDeadAnimFinished())
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
	}

	//プレイヤーと敵の当たり判定
	Enemy* hitEnemy = CollisionManager::PlayerVsEnemies(
		pPlayer_->GetColRect(),
		enemyFactory_.GetEnemies());

	//敵に当たっていてプレイヤーが死んでいない場合ダメージ処理を行う
	//ただしボス2の場合は当たり判定が別途あるため除外する
	if (hitEnemy && !pPlayer_->IsDead())
	{
		if (dynamic_cast<Boss2*>(hitEnemy) == nullptr)
		{
			pPlayer_->OnDamage(hitEnemy->GetPos().x);
		}
	}

	auto boss1 = enemyFactory_.GetBoss1();
	if (boss1 && boss1->IsDeadAnimFinished() && clearState_ == ClearState::None)
	{
		clearState_ = ClearState::BossCameraShake;

		// ボス1撃破フラグ
		controller_.GetProgress().isDefeatedBoss1_ = true;

		// カメラ揺れ
		pCamera_->Shake(30, 6.0f);
	}

	//プレイヤーとボス2の当たり判定
	if (boss2)
	{
		if (!pPlayer_->IsDead())
		{
			if (CollisionManager::PlayerVsBoss2(
				pPlayer_->GetColRect(), *boss2))
			{
				pPlayer_->OnDamage(boss2->GetPos().x);
			}
		}
	}

	//チュートリアルステージのゴール判定処理
	if (tutorialManager_)
	{
		const auto& goalRect = tutorialManager_->GetGoalRect();
		const auto& playerRect = pPlayer_->GetColRect();

		if (goalRect.IsCollision(playerRect))
		{
			//ゴール到達したらフェードアウトして次のシーンへ遷移
			update_ = &GameScene::GoalFadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
			return;
		}
	}

	//ボスを倒すとクリアシーンに遷移する
	if (boss2)
	{
		// ボスが死亡アニメ終了していたらクリア処理開始
		if (boss2->IsDeadAnimFinished() && clearState_ == ClearState::None)
		{
			controller_.ChangeScene(std::make_shared<ClearScene>(controller_));
		}
	}

	//クリア状態がボスを倒した後のカメラ揺らし状態の場合は
	//カメラの揺れが終了するまで待機する
	if (clearState_ == ClearState::BossCameraShake)
	{
		if (!pCamera_->IsShaking())
		{
			clearState_ = ClearState::AutoWalk;

			autoWalkStartX_ = pPlayer_->GetPos().x;
			pPlayer_->StartAutoWalk(kPlayerDir);
		}
	}

	//クリア状態が自動歩行状態の場合は
	//一定距離進んだらクリアシーンへ遷移する
	if (clearState_ == ClearState::AutoWalk)
	{
		float nowX = pPlayer_->GetPos().x;

		if (nowX >= autoWalkStartX_ + kPlayerAutoWalkX)
		{
			update_ = &GameScene::GoalFadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
			return;
		}
	}

#ifdef _DEBUG
	//デバッグ用：ステージクリア
	if (input.IsTriggered("debug_warp"))
	{
		pPlayer_->SetPos(Vector2{ 8300,1740 });
	}
#endif
}

void GameScene::FadeOutUpdate(Input&) 
{
	if (frame_++ >= fade_interval)
	{
		controller_.ChangeScene(std::make_shared<GameOverScene>(controller_,stageType_));
		return;
	}
}

void GameScene::GoalFadeOutUpdate(Input&)
{
	if (frame_++ >= fade_interval)
	{
		//次のステージを取得
		StageType next = GetNextStageType(stageType_);
		controller_.ChangeScene(std::make_shared<GameScene>(controller_,next));
	}
}

void GameScene::FadeDraw() 
{
	NormalDraw();

	//フェードの描画
	auto rate = static_cast<float>(frame_) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast <int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}

void GameScene::NormalDraw() 
{
	if (stageType_ == StageType::Stage2)
	{
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight,0x00ff00, true);
	}

	Vector2 cameraOffset = pCamera_->GetOffset();

	//各クラスの描画処理
	bg_->Draw(pCamera_);
	pPlayer_->SetCameraOffset(cameraOffset);
	pPlayer_->Draw();

	enemyFactory_.Draw(cameraOffset);

	bulletManager_.SetCameraOffset(cameraOffset);
	bulletManager_.Draw();

	effectManager_.SetCameraOffset(cameraOffset);
	effectManager_.Draw();

	//UIの描画
	weaponUI_.Draw();

	//チュートリアルステージの描画
	if (tutorialManager_)
	{
		tutorialManager_->Draw(*pCamera_);
	}
}

void GameScene::Init()
{
	//ゲーム進行状況の取得
	gameProgress_ = &controller_.GetProgress();

	//各クラスの初期化
	bg_ = std::make_shared<Bg>(stageType_);
	bg_->Init();

	//プレイヤーの初期化
	pPlayer_ = std::make_shared<Player>(Vector2{ kPlayerSpawnPosX, kPlayerSpawnPosY }, Vector2{ 0,0 });
	pPlayer_->Init();
	pPlayer_->SetCamera(pCamera_);
	pPlayer_->SetBg(bg_);
	pPlayer_->SetGameProgress(gameProgress_);

	//カメラの初期化
	pCamera_->Init(pPlayer_);

	bulletManager_.SetCamera(pCamera_.get());

	//敵の初期化
	enemyFactory_.SetEffectManager(&effectManager_);
	enemyFactory_.LoadFromCSV(stageType_, &bulletManager_);
	if (stageType_ == StageType::Stage1)
	{
		enemyFactory_.AddBoss1(Vector2{ kBoss1SpawnPosX,kBoss1SpawnPosY }, Vector2{ 0,0 },
			pPlayer_, &bulletManager_, pCamera_);
	}
	else if (stageType_ == StageType::Stage2)
	{
		BulletType weapon = pPlayer_->GetCurrentBulletType();

		enemyFactory_.AddBoss2(Vector2{ kBoss2SpawnPosX,kBoss2SpawnPosY },Vector2{ 0,0 },
			pPlayer_,&bulletManager_,pCamera_,weapon);
	}
	else if (stageType_ == StageType::BossDebugStage)
	{
		BulletType weapon = pPlayer_->GetCurrentBulletType();

		enemyFactory_.AddBoss2(Vector2{ kBoss2SpawnPosX, kBoss2SpawnPosY }, Vector2{ 0,0 },
			pPlayer_, &bulletManager_, pCamera_, weapon);
	}
	enemyFactory_.Init(pPlayer_, bg_);

	//UIの初期化
	weaponUI_.Init();

	//BGMの再生
	Application::GetInstance().GetBGMManager().PlayBGM(BGM::Game);

	//たいまつのアンロックするための処理
	auto& progress = controller_.GetProgress();

	if (stageType_ == StageType::Stage2 && progress.isDefeatedBoss1_)
	{
		pPlayer_->UnlockTorch();
	}

	//チュートリアルステージ用の初期化
	if (stageType_ == StageType::Tutorial)
	{
		tutorialManager_ = std::make_unique<TutorialManager>();
		tutorialManager_->Init();

		enemyFactory_.LoadFromCSV(StageType::Tutorial, &bulletManager_);
		enemyFactory_.Init(pPlayer_, bg_);
	}
	else
	{
		tutorialManager_.reset();
	}
}

void GameScene::Update(Input& input)
{
	(this->*update_)(input);
}

void GameScene::Draw()
{
	(this->*draw_)();
}

//次のステージタイプを取得
StageType GameScene::GetNextStageType(StageType nextStage)
{
	switch (nextStage)
	{
	case StageType::Tutorial: 
		return StageType::Stage1;
	case StageType::Stage1:   
		return StageType::Stage2;
	default:                 
		return nextStage;
	}
}