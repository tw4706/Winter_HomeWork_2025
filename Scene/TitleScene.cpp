
#include "TitleScene.h"
#include<Dxlib.h>
#include"Input.h"
#include"Application.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "SceneController.h"
#include"GlobalConstants.h"

constexpr int kFadeInterval = 60;

void TitleScene::FadeInUpdate(Input&input)
{
	if (--frame_ <= 0)
	{
		update_ = &TitleScene::NormalUpdate;
		draw_ = &TitleScene::NormalDraw;
		return;
	}
}

void TitleScene::NormalUpdate(Input&input)
{
	if (input.IsTriggered("next")) {
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		frame_ = 0;// フェードアウトの最初
		return;
	}
}

void TitleScene::FadeOutUpdate(Input&input)
{
	if (frame_++ >= kFadeInterval) {
		controller_.ChangeScene(std::make_shared<SelectScene>(controller_));
		return;
	}
}

void TitleScene::NormalDraw()
{
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight/2-100 , 1.0f, 0.0f, titleH_, true);
}

void TitleScene::FadeDraw()
{
	// 値の範囲を一旦0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = 1.0f-static_cast<float>(frame_) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);//αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);// ブレンドしない
}

TitleScene::TitleScene(SceneController&controller):Scene(controller)
{
	titleH_ = LoadGraph("data/map/title.png");
	update_ = &TitleScene::FadeInUpdate;
	draw_ = &TitleScene::FadeDraw;
	frame_ = kFadeInterval;
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
}

void TitleScene::Update(Input&input)
{
	(this->*update_)(input);
}

void TitleScene::Draw()
{
	(this->*draw_)();
}
