#include "TitleScene.h"
#include<Dxlib.h>
#include"Input.h"
#include"SceneController.h"
#include"Application.h"

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

void TitleScene::FadeOutUpdate(Input&)
{
	if (frame_++ >= fade_interval) {
		controller_.ChangeScene(std::make_shared<GameScene>(controller_));
		return;
	}
}

void TitleScene::NormalDraw()
{

}

void TitleScene::FadeDraw()
{
}

TitleScene::TitleScene(SceneController&)
{
}

void TitleScene::Update(Input&input)
{
}

void TitleScene::Draw()
{
}
