
#include "TitleScene.h"
#include<Dxlib.h>
#include"Input.h"
#include"StageType.h"
#include"Application.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "SceneController.h"
#include"GlobalConstants.h"

namespace
{
	constexpr int kFadeInterval = 60;

	//確認バナー幅・高さ
	constexpr int kBannerW = 400;
	constexpr int kBannerH = 150;
	//確認バナー表示位置
	constexpr int kBannerX = Game::kScreenWidth / 2 - kBannerW / 2;
	constexpr int kBannerY = Game::kScreenHeight / 2 - kBannerH / 2;
	constexpr int kConfirmMessageX = kBannerX + 20;
	constexpr int kConfirmMessageY = kBannerY + 20;
	constexpr int kOptionOffsetY = kBannerY + 70;
	constexpr int kOptionYesOffsetX = kBannerX + 50;
	constexpr int kOptionNoOffsetX = kBannerX + 250;

}


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
	if (input.IsTriggered("next")) 
	{
#ifdef _DEBUG
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		frame_ = 0;// フェードアウトの最初
#else
		titleState_ = TitleState::Confirm;
		confirmSelect_ = 0;
		update_ = &TitleScene::ConfirmUpdate;
		draw_ = &TitleScene::ConfirmDraw;
#endif
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

void TitleScene::ConfirmUpdate(Input&input)
{
	//選択変更
	if (input.IsTriggered("left") || input.IsTriggered("up")) confirmSelect_ = 0;
	if (input.IsTriggered("right") || input.IsTriggered("down")) confirmSelect_ = 1;

	//決定
	if (input.IsTriggered("next"))
	{
		if (confirmSelect_ == 0)
		{
			auto stageType = StageType::Stage2;
			// はい → チュートリアル
			controller_.ChangeScene(std::make_shared<GameScene>(controller_,stageType));
		}
		else
		{
			auto stageType = StageType::Stage1;
			// いいえ → ステージ1
			controller_.ChangeScene(std::make_shared<GameScene>(controller_, stageType));
		}
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

void TitleScene::ConfirmDraw()
{
	// 背景は通常のタイトルを描画
	NormalDraw();

	// 半透明背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBox(kBannerX, kBannerY, kBannerX + kBannerW, kBannerY + kBannerH, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//メッセージ
	DrawString(kConfirmMessageX, kConfirmMessageY, "このゲームを初めて遊びますか？", 0xffffff);

	// 選択肢
	DrawString(kOptionYesOffsetX, kOptionOffsetY, "はい", confirmSelect_ == 0 ? 0xff0000 : 0xffffff);
	DrawString(kOptionNoOffsetX, kOptionOffsetY, "いいえ", confirmSelect_ == 1 ? 0xff0000 : 0xffffff);
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller),
	titleH_(-1),
	frame_(0),
	titleState_(TitleState::Normal),
	confirmSelect_(0)
{
	update_ = &TitleScene::FadeInUpdate;
	draw_ = &TitleScene::FadeDraw;
}

TitleScene::~TitleScene()
{
	DeleteGraph(titleH_);
}

void TitleScene::Init()
{
	titleH_ = LoadGraph("data/map/title.png");
	frame_ = kFadeInterval;
}

void TitleScene::Update(Input&input)
{
	(this->*update_)(input);
}

void TitleScene::Draw()
{
	(this->*draw_)();
}
