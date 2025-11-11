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
	player_->Init();

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

	player_->Update(input);
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}

void GameScene::Draw()
{
	(this->*draw_)();

	player_->Draw();
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}
