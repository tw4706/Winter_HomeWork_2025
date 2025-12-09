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
#include"GlobalConstants.h"
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

GameScene::GameScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	keyH_(-1),
	frame_(0)
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
	// 値の範囲を一旦0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = static_cast<float>(frame_) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);// αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);// ブレンドしない
}

void GameScene::NormalDraw() 
{
	Vector2 cameraOffset = camera_->GetOffset();

	//各クラスの描画処理
	bg_->Draw(camera_);
	player_->SetCameraOffset(cameraOffset);
	player_->Draw();

	enemyFactory_.Draw(cameraOffset);

	bulletManager_.SetCameraOffset(cameraOffset);
	bulletManager_.Draw();

	//鍵画像を描画 magicNumber
	DrawGraph(
		static_cast<int>(8500 - cameraOffset.x),
		static_cast<int>(1724 - cameraOffset.y),
		keyH_, TRUE
	);

#ifdef _DEBUG
	Rect goalRectScreen = goalRect_;
	goalRectScreen.left_ -= cameraOffset.x;
	goalRectScreen.top_ -= cameraOffset.y;
	goalRectScreen.right_ -= cameraOffset.x;
	goalRectScreen.bottom_ -= cameraOffset.y;
	goalRectScreen.Draw(0xff0000, false);
#endif
}

void GameScene::Init()
{
	//各クラスの初期化
	bg_->Init();

	float spawnPosX = 50.0f;
	float spawnPosY = 1744.0f;

	switch (stageType_)
	{
	case StageType::Stage1:


		player_ = std::make_shared<Player>(Vector2{ spawnPosX,spawnPosY }, Vector2{});

		enemyFactory_.LoadFromCSV("data/Enemy/enemyData.csv", &bulletManager_);
		goalRect_.SetLT(8500, 1724, 32, 32);

		break;
	case StageType::BossDebugStage:
		player_ = std::make_shared<Player>(Vector2{ 100,300 }, Vector2{});
		enemyFactory_.AddBoss(Vector2{ 300,300 },player_,bg_);
		break;
	default:
		break;
	}

	player_->Init();
	player_->SetBg(bg_);

	camera_->Init(player_);

	enemyFactory_.Init(player_, bg_);

	keyH_ = LoadGraph("data/map/Key.png");
	assert(keyH_ >= 0);
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
	bulletManager_.Update(input,enemyFactory_.GetEnemies(), *player_);

	if (player_->IsDead()&&player_->IsDeadAnimFinished())
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;

		return;
	}

	//プレイヤーと敵の当たり判定
	for (auto& enemy : enemyFactory_.GetEnemies())
	{
		if (!enemy->IsDead() && player_->GetColRect().IsCollision(enemy->GetColRect()))
		{
			if (!player_->IsDead())
			{
				player_->Dead();
			}
			break;
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
}
