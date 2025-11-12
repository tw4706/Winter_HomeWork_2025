#include "GameScene.h"
#include<Dxlib.h>
#include"Input.h"
#include"Player.h"
#include"Zombie.h"
#include"SceneController.h"
#include"Application.h"
#include<cassert>
#include<cmath>

namespace 
{
	//フェードまでの間隔
	constexpr int fade_interval = 60;
}

GameScene::GameScene(SceneController& controller) :
	Scene(controller),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw)
{
	player_ = std::make_shared<Player>(Vector2{ 100,500 }, Vector2{});
	enemies_.push_back(std::make_shared<Zombie>(Vector2{ 800,450 }, Vector2{}));
	enemies_.push_back(std::make_shared<Dog>(Vector2{ 1000,500 }, Vector2{}));
}

void GameScene::FadeInUpdate(Input&) {
	if (frame_-- <= 0) {
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
		return;
	}
}

void GameScene::NormalUpdate(Input& input)
{

}


void GameScene::FadeOutUpdate(Input&) {
	if (frame_++ >= fade_interval)
	{
//		controller_.ChangeScene(std::make_shared<GameOverScene>(controller_));
		return;
	}
}

void GameScene::FadeDraw() {

}

void GameScene::NormalDraw() {

}

void GameScene::Init()
{
	//プレイヤーの初期化処理
	player_->Init();

	//敵の初期化処理
	for (auto& enemy : enemies_)
	{
		enemy->Init();
		enemy->SetPlayer(player_);
	}
}

void GameScene::Update(Input& input)
{
	(this->*update_)(input);

	if (update_ != &GameScene::NormalUpdate) return;

	//プレイヤーの更新処理
	player_->Update(input,enemies_);
	//敵の更新処理
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}

void GameScene::Draw()
{
	(this->*draw_)();

	//プレイヤーの描画処理
	player_->Draw();
	//敵の描画処理
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}
