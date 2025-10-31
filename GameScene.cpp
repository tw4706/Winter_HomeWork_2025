#include "GameScene.h"
#include<Dxlib.h>
#include"Input.h"
//#include"GameOverScene.h"
#include"SceneController.h"
#include"Application.h"
#include<cassert>
#include<cmath>

namespace 
{
	constexpr int fade_interval = 60;
}

GameScene::GameScene(SceneController& controller):
	Scene(controller),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw)
{
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
	}
}

void GameScene::FadeDraw() {

}

void GameScene::NormalDraw() {

}

void GameScene::Update(Input&input)
{
	(this->*update_)(input);
}

void GameScene::Draw()
{
	(this->*draw_)();
}
