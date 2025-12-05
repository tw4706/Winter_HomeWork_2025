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
#include"SceneController.h"
#include<cmath>
#include<cassert>
#include<Dxlib.h>

namespace 
{
	//フェードまでの間隔
	constexpr int fade_interval = 60;

	//落下判定となる座標
	constexpr float kFallLimit = 1900.0f;
}

GameScene::GameScene(SceneController& controller) :
	Scene(controller),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw)
{
	bg_ = std::make_shared<Bg>();
	camera_ = std::make_shared<Camera>();
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
	if (player_->GetPos().y > kFallLimit)
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
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
		controller_.ChangeScene(std::make_shared<ClearScene>(controller_));
	}
}

void GameScene::FadeDraw() 
{
}

void GameScene::NormalDraw() 
{
}

void GameScene::Init()
{
	//各クラスの初期化
	bg_->Init();


	float spawnPosX = 50.0f;
	float spawnPosY = 1744.0f;

	player_ = std::make_shared<Player>(Vector2{ spawnPosX,spawnPosY }, Vector2{});
	player_->Init();
	player_->SetBg(bg_);

	camera_->Init(player_);

	enemyFactory_.LoadFromCSV("data/Enemy/enemyData.csv", &bulletManager_);
	enemyFactory_.Init(player_, bg_);

	goalRect_.SetLT(8500, 1744, 100, 200);
}

void GameScene::Update(Input& input)
{
	(this->*update_)(input);

	if (update_ != &GameScene::NormalUpdate) return;

	//各クラスの更新処理
	camera_->Update(player_);

	player_->Update(input,bulletManager_);

	enemyFactory_.Update();

	//弾の更新処理
	bulletManager_.Update(enemyFactory_.GetEnemies(), *player_);

	//プレイヤーと敵の当たり判定
	for (auto& enemy : enemyFactory_.GetEnemies())
	{
		if (!enemy->IsDead() && player_->GetColRect().IsCollision(enemy->GetColRect()))
		{
			player_->OnDamage();
			//printfDx("PlayerHit\n");

			update_ = &GameScene::GoalFadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
		}
	}

	//ゴールとの当たり判定
	if (player_->GetColRect().IsCollision(goalRect_))
	{
		printfDx("GoalHit!\n");

		update_ = &GameScene::GoalFadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
	}
}

void GameScene::Draw()
{
	(this->*draw_)();

	//各クラスの描画処理
	Vector2 cameraOffset=camera_->GetOffset();
	bg_->Draw(camera_);
	player_->SetCameraOffset(cameraOffset);
	player_->Draw();

	enemyFactory_.Draw(cameraOffset);

	bulletManager_.SetCameraOffset(cameraOffset);
	bulletManager_.Draw();
}
