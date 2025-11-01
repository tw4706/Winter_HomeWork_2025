#include "TitleScene.h"
#include<Dxlib.h>
#include"Input.h"
#include"Game.h"
#include"SceneController.h"
#include"Application.h"
#include "GameScene.h"

constexpr int fade_interval = 60;

void TitleScene::FadeInUpdate(Input&)
{
	if (frame_-- <= 0)
	{
		update_ = &TitleScene::NormalUpdate;
		draw_ = &TitleScene::NormalDraw;
		return;
	}
}

void TitleScene::NormalUpdate(Input&input)
{
	if (input.IsTriggered("ok")) {
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		frame_ = 0;// フェードアウトの最初
		return;
	}
}

void TitleScene::FadeOutUpdate(Input&input)
{
	if (frame_++ >= fade_interval) {
		controller_.ChangeScene(std::make_shared<GameScene>(controller_));
		return;
	}
}

void TitleScene::NormalDraw()
{
	DrawRotaGraph(Game::kScreenWidth, Game::kScreenHeight, 1.0f, 0.0f, titleH_, true);
}

void TitleScene::FadeDraw()
{
	DrawRotaGraph(Game::kScreenWidth, Game::kScreenHeight, 1.0f, 0.0f, titleH_, true);
	// 値の範囲を一旦0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = static_cast<float>(frame_) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);// αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);// ブレンドしない
}

TitleScene::TitleScene(SceneController&controller):Scene(controller)
{
	titleH_ = LoadGraph("data/title.png");
	update_ = &TitleScene::FadeInUpdate;
	draw_ = &TitleScene::FadeDraw;
	frame_ = fade_interval;
}

void TitleScene::Update(Input&input)
{
	(this->*update_)(input);
}

void TitleScene::Draw()
{
	(this->*draw_)();
}
