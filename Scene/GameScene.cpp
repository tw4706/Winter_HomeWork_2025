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

	constexpr float kBoss1SpawnPosX = 8800.0f;
	constexpr float kBoss1SpawnPosY = 1600.0f;

	//フェードまでの間隔
	constexpr int fade_interval = 60;
	constexpr float kColSize = 32;

	//落下判定となる座標
	constexpr float kFallLimit = 1900.0f;

	//鍵が落ちる速度
	constexpr int kKeyDropSpeed = 5.0f;
	constexpr float kKeyAttractRange = 300.0f; // 吸い付く距離
	constexpr float kKeyAttractSpeed = 12.0f; // 吸引速度
}

GameScene::GameScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	keyH_(-1),
	frame_(fade_interval),
	isKeyActive_(false),
	keyPos_{0,0}
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
		if (enemy->IsBoss() && enemy->IsDead())
		{
			if (!isKeyActive_)
			{
				isKeyActive_ = true;
				keyPos_ = Vector2{ enemy->GetPos().x, enemy->GetPos().y - 200 }; //上から落とす位置
			}
		}
	}

	//鍵を出現させ、取得するとクリアシーンに遷移する
	if (isKeyActive_)
	{
		Vector2 playerPos = pPlayer_->GetPos();

		float dx = playerPos.x - keyPos_.x;
		float dy = playerPos.y - keyPos_.y;
		float dist = std::sqrt(dx * dx + dy * dy);

		if (dist < kKeyAttractRange)
		{
			// プレイヤーに吸い付く
			if (dist > 0.01f)
			{
				dx /= dist;
				dy /= dist;
			}
			keyPos_.x += dx * kKeyAttractSpeed;
			keyPos_.y += dy * kKeyAttractSpeed;
		}
		else
		{
			// 通常落下
			keyPos_.y += kKeyDropSpeed;
		}

		keyRect_.SetCenter(keyPos_.x, keyPos_.y, 32, 32);

		if (CollisionManager::PlayerVsKey(pPlayer_->GetColRect(),keyRect_))
		{
			isKeyActive_ = false;
			update_ = &GameScene::GoalFadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
		}
	}
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

	int keyScreenX = keyRect_.left_ + cameraOffset.x;
	int keyScreenY = keyRect_.top_ + cameraOffset.y;

	if (isKeyActive_)
	{
		//鍵画像を描画
		DrawRectRotaGraph3(keyScreenX, keyScreenY,
			0, 0,
			32, 32,
			16, 16,
			1.0f, 1.0f,
			0.0,
			keyH_, TRUE);
	}

	//ステージ2表示
	DrawFormatString(Game::kScreenWidth, Game::kScreenHeight,
		GetColor(255, 255, 0),"STAGE %d",static_cast<int>(stageType_) + 1);

#ifdef _DEBUG
	Rect screenGoal = keyRect_;
	screenGoal.left_ -= cameraOffset.x;
	screenGoal.top_ -= cameraOffset.y;
	screenGoal.right_ -= cameraOffset.x;
	screenGoal.bottom_ -= cameraOffset.y;
	screenGoal.Draw(0xff0000, false);
#endif
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
		enemyFactory_.AddBoss(Vector2{ kBoss1SpawnPosX,kBoss1SpawnPosY }, Vector2{ 0,0 }, pPlayer_, &bulletManager_, pCamera_);
	}
	if (stageType_ == StageType::BossDebugStage)
	{
		enemyFactory_.AddBoss(Vector2{ 1000,1600 }, Vector2{ 0,0 }, pPlayer_, &bulletManager_, pCamera_);
	}
	enemyFactory_.Init(pPlayer_, bg_);

	keyH_ = LoadGraph("data/map/Key.png");
	assert(keyH_ >= 0);
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
