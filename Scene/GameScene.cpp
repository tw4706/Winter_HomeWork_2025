#include "GameScene.h"
#include"Bg.h"
#include"Dog.h"
#include"Rect.h"
#include"Input.h"
#include"Player.h"
#include"Zombie.h"
#include"Application.h"
#include "SkullFlower.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "SelectScene.h"
#include "PauseScene.h"
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
	constexpr float kBoss2SpawnPosX = 1000.0f;
	constexpr float kBoss2SpawnPosY = 1600.0f;

	//フェードまでの間隔
	constexpr int fade_interval = 60;
	constexpr float kColSize = 32;

	//落下判定となる座標
	constexpr float kFallLimit = 1900.0f;

	//プレイヤーが自動で歩行する向きと距離
	constexpr int kPlayerDir = 1;//右向き
	constexpr float kPlayerAutoWalkX = 600.0f;
}

GameScene::GameScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	clearState_(ClearState::None),
	frame_(fade_interval),
	isBossDefeated_(false),
	autoWalkStartX_(0)
{
	//bgにステージに対応するマップデータをセット
	bg_ = std::make_shared<Bg>(static_cast<int>(stage) + 1);
	pCamera_ = std::make_shared<Camera>();
}

void GameScene::FadeInUpdate(Input&)
{
	if (frame_-- <= 0) 
	{
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
		return;
	}
}

void GameScene::NormalUpdate(Input&input)
{
	if (input.IsTriggered("next"))
	{
		controller_.ChangeScene(std::make_shared<SelectScene>(controller_));
		return;
	}

	//ポーズボタンを押したらポーズシーンに遷移
	//if (input.IsTriggered("pause"))
	//{
	//	controller_.PushScene(std::make_shared<PauseScene>(controller_));
	//	return;
	//}

	if (pPlayer_->GetPos().y > kFallLimit)
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
	}

	//各クラスの更新処理
	pCamera_->Update(pPlayer_);

	pPlayer_->Update(input, bulletManager_,stageType_);

	enemyFactory_.Update();

	//弾の更新処理
	bulletManager_.Update(input, enemyFactory_.GetEnemies(), *pPlayer_);

	//プレイヤーの弾 × 敵の当たり判定
	CollisionManager::PlayerBulletsVsEnemies(
		bulletManager_.GetBullets(),enemyFactory_.GetEnemies());

	//敵の弾 × プレイヤーの当たり判定
	CollisionManager::EnemyBulletsVsPlayer(bulletManager_.GetBullets(),*pPlayer_);


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

	//敵に当たっていて、プレイヤーが死んでいない場合ダメージ処理を行う
	if (hitEnemy && !pPlayer_->IsDead())
	{
		pPlayer_->OnDamage(hitEnemy->GetPos().x);
	}


	//ボスを倒すとクリアシーンに遷移
	for (auto& enemy : enemyFactory_.GetEnemies())
	{
		//敵がボス+死亡している場合
		if (enemy->IsBoss() && enemy->IsDead()
			&& clearState_ == ClearState::None)
		{
			clearState_ = ClearState::BossCameraShake;

			//ボスを倒したことをプレイヤーに伝える
			controller_.GetProgress().isDefeatedBoss1_ = true;
		}
	}

	if (clearState_ == ClearState::BossCameraShake)
	{
		if (!pCamera_->IsShaking())
		{
			clearState_ = ClearState::AutoWalk;

			autoWalkStartX_ = pPlayer_->GetPos().x;
			pPlayer_->StartAutoWalk(kPlayerDir);
		}
	}

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
		controller_.ChangeScene(std::make_shared<GameOverScene>(controller_));
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
	//フェードの描画
	auto rate = static_cast<float>(frame_) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
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

	//ステージ2表示
	DrawFormatString(Game::kScreenWidth, Game::kScreenHeight,
		GetColor(255, 255, 0),"STAGE %d",static_cast<int>(stageType_) + 1);
}

void GameScene::Init()
{
	//各クラスの初期化
	bg_ = std::make_shared<Bg>(static_cast<int>(stageType_) + 1);
	bg_->Init();

	//プレイヤーのスポーン位置位置

	//プレイヤーの初期化
	pPlayer_ = std::make_shared<Player>(Vector2{ kPlayerSpawnPosX, kPlayerSpawnPosY }, Vector2{ 0,0 });
	pPlayer_->Init();
	pPlayer_->Init();
	pPlayer_->SetBg(bg_);

	//カメラの初期化
	pCamera_->Init(pPlayer_);

	//敵の初期化
	enemyFactory_.LoadFromCSV(stageType_, &bulletManager_);
	if (stageType_ == StageType::Stage1)
	{
		enemyFactory_.AddBoss1(Vector2{ kBoss1SpawnPosX,kBoss1SpawnPosY }, Vector2{ 0,0 },
			pPlayer_, &bulletManager_, pCamera_);
	}
	else if (stageType_ == StageType::Stage2)
	{
		enemyFactory_.AddBoss2(Vector2{ kBoss2SpawnPosX, kBoss2SpawnPosY },Vector2{ 0,0 },
			pPlayer_,&bulletManager_,pCamera_);
	}
	else if (stageType_ == StageType::BossDebugStage)
	{
		enemyFactory_.AddBoss2(Vector2{ kBoss2SpawnPosX, kBoss2SpawnPosY },Vector2{ 0,0 },
			pPlayer_,&bulletManager_,pCamera_);	}
	enemyFactory_.Init(pPlayer_, bg_);


	//たいまつのアンロックするための処理
	auto& progress = controller_.GetProgress();

	if (stageType_ == StageType::Stage2 && progress.isDefeatedBoss1_)
	{
		pPlayer_->UnlockTorch();
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

StageType GameScene::GetNextStageType(StageType nextStage)
{
	switch (nextStage)
	{
	case StageType::Stage1: return StageType::Stage2;
	default: return nextStage;
	}
}
