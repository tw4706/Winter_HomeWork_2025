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
#include"SceneController.h"
#include"GlobalConstants.h"
#include<cmath>
#include<cassert>
#include<Dxlib.h>

namespace 
{
	//フェードまでの間隔
	constexpr int fade_interval = 60;
	constexpr float kColSize = 32;

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
	if (update_ != &GameScene::NormalUpdate) return;

	if (input.IsTriggered("next"))
	{
		controller_.ChangeScene(std::make_shared<SelectScene>(controller_));
		return;
	}

	if (pPlayer_->GetPos().y > kFallLimit)
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
	}

	//各クラスの更新処理
	pCamera_->Update(pPlayer_);

	pPlayer_->Update(input, bulletManager_);

	enemyFactory_.Update();

	//弾の更新処理
	bulletManager_.Update(input, enemyFactory_.GetEnemies(), *pPlayer_);

	if (pPlayer_->IsDead() && pPlayer_->IsDeadAnimFinished())
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;

		return;
	}

	//プレイヤーと敵の当たり判定
	for (auto& enemy : enemyFactory_.GetEnemies())
	{
		if (!enemy->IsDead() && pPlayer_->GetColRect().IsCollision(enemy->GetColRect()))
		{
			if (!pPlayer_->IsDead())
			{
				pPlayer_->Dead();
			}
			break;
		}
	}

	//ゴールとの当たり判定
	if (pPlayer_->GetColRect().IsCollision(goalRect_))
	{
		printfDx("GoalHit!\n");
		update_ = &GameScene::GoalFadeOutUpdate;
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
	//フェードの描画
	auto rate = static_cast<float>(frame_) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}

void GameScene::NormalDraw() 
{
	Vector2 cameraOffset = pCamera_->GetOffset();

	//各クラスの描画処理
	bg_->Draw(pCamera_);
	pPlayer_->SetCameraOffset(cameraOffset);
	pPlayer_->Draw();

	enemyFactory_.Draw(cameraOffset);

	bulletManager_.SetCameraOffset(cameraOffset);
	bulletManager_.Draw();

	int keyScreenX = goalRect_.left_ + cameraOffset.x;
	int keyScreenY = goalRect_.top_ + cameraOffset.y;

	//鍵画像を描画 magicNumber
	DrawRectRotaGraph3(keyScreenX, keyScreenY,
		0,0,
		32,32,
		16,16,
		1.0f,1.0f,
		0.0,
		keyH_, TRUE);

#ifdef _DEBUG
	Rect screenGoal = goalRect_;
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
	bg_->Init();

	float spawnPosX = 50.0f;
	float spawnPosY = 1744.0f;

	switch (stageType_)
	{
	case StageType::Stage1:


		pPlayer_ = std::make_shared<Player>(Vector2{ spawnPosX,spawnPosY }, Vector2{});

		enemyFactory_.LoadFromCSV("data/Enemy/enemyData.csv", &bulletManager_);
		goalRect_.SetLT(8500, 1724, kColSize, kColSize);

		break;
	case StageType::BossDebugStage:
		pPlayer_ = std::make_shared<Player>(Vector2{ spawnPosX,spawnPosY }, Vector2{});
		enemyFactory_.AddBoss(Vector2{ 1000,1600 }, Vector2{0,0},pPlayer_, &bulletManager_, pCamera_);
		break;
	default:
		break;
	}

	pPlayer_->Init();
	pPlayer_->SetBg(bg_);

	pCamera_->Init(pPlayer_);

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
