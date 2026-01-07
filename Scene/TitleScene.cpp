
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

	constexpr int kTitleScale = 1.0f;

	constexpr float kPressStartScale = 0.25f;
	constexpr int kDecideBlinkFrame = 20;

	//確認バナー幅と高さ
	constexpr int kBannerW = 400;
	constexpr int kBannerH = 150;
	//確認バナー表示位置
	constexpr int kBannerX = Game::kScreenWidth / 2 - kBannerW / 2;
	constexpr int kBannerY = Game::kScreenHeight / 2 - kBannerH / 2;
	constexpr int kConfirmMessageX = kBannerX + 75;
	constexpr int kConfirmMessageY = kBannerY + 20;
	constexpr int kOptionOffsetY = kBannerY + 70;
	constexpr int kOptionYesOffsetX = kBannerX + 90;
	constexpr int kOptionNoOffsetX = kBannerX + 290;

	//選択矢印の位置オフセット
	constexpr int kArrowOffsetX = -20;
	constexpr int kArrowOffsetY = 3;
	constexpr float kArrowScale = 1.0f;
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
	//決定中の高速点滅更新
	if (isDeciding_)
	{
		pressBlinkFrame_++;
		decideBlinkCount_++;

		if (decideBlinkCount_ >= kDecideBlinkFrame)
		{
			// 点滅終了 → 確認画面へ
			titleState_ = TitleState::Confirm;
			confirmSelect_ = 0;

			isDeciding_ = false;
			decideBlinkCount_ = 0;

			update_ = &TitleScene::ConfirmUpdate;
			draw_ = &TitleScene::ConfirmDraw;
		}
		return;
	}

	//通常時の点滅更新
	pressBlinkFrame_++;

	if (input.IsTriggered("next"))
	{
#ifdef _DEBUG
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		return;
#endif
		isDeciding_ = true;
		pressBlinkFrame_ = 0;
		decideBlinkCount_ = 0;
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
			auto stageType = StageType::Tutorial;
			//はいの場合はチュートリアル
			controller_.ChangeScene(std::make_shared<GameScene>(controller_,stageType));
		}
		else
		{
			auto stageType = StageType::Stage1;
			//いいえの場合はステージ1
			controller_.ChangeScene(std::make_shared<GameScene>(controller_, stageType));
		}
	}
}

void TitleScene::NormalDraw()
{
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight/2-100 , kTitleScale, 0.0f, titleH_, true);

	//PressStartUIの点滅表示
	if (isDeciding_)
	{
		//決定時の小刻み点滅
		bool visible = ((pressBlinkFrame_ / 3) % 2) == 0;

		if (visible)
		{
			DrawRotaGraph(
				Game::kScreenWidth / 2,
				Game::kScreenHeight / 2 + 100,
				kPressStartScale,
				0.0f,
				pressStartH_,
				true);
		}
	}
	else
	{
		//通常時の点滅
		int alpha = static_cast<int>(128 + 127 * sinf(pressBlinkFrame_ * 0.05f));

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		DrawRotaGraph(
			Game::kScreenWidth / 2,
			Game::kScreenHeight / 2 + 100,
			kPressStartScale,
			0.0f,
			pressStartH_,
			true);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
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

	int arrowX = (confirmSelect_ == 0)
		? kOptionYesOffsetX
		: kOptionNoOffsetX;

	int arrowY = kOptionOffsetY;

	DrawRotaGraph(
		arrowX + kArrowOffsetX,
		arrowY + kArrowOffsetY,
		kArrowScale,
		DX_PI_F / 2.0f,
		selectH_,
		true);
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller),
	titleH_(-1),
	pressStartH_(-1),
	selectH_(-1),
	frame_(0),
	pressBlinkFrame_(0),
	isDeciding_(false),
	decideBlinkCount_(0),
	titleState_(TitleState::Normal),
	confirmSelect_(0)
{
	update_ = &TitleScene::FadeInUpdate;
	draw_ = &TitleScene::FadeDraw;
}

TitleScene::~TitleScene()
{
	DeleteGraph(titleH_);
	DeleteGraph(pressStartH_);
	DeleteGraph(selectH_);
}

void TitleScene::Init()
{
	titleH_ = LoadGraph("data/UI/title.png");
	pressStartH_ = LoadGraph("data/UI/PressButton.png");
	selectH_ = LoadGraph("data/Bullet/Lance.png");
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
