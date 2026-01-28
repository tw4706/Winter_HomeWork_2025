
#include "TitleScene.h"
#include<Dxlib.h>
#include"Bg.h"
#include"Input.h"
#include"Player.h"
#include"Zombie.h"
#include"GameScene.h"
#include"StageType.h"
#include"DemoPlayer.h"
#include"DemoZombie.h"
#include"Application.h"
#include"SelectScene.h"
#include"BulletManager.h"
#include"SceneController.h"
#include"GlobalConstants.h"
#include "EffekseerForDXLib.h"

namespace
{
	constexpr int kFadeInterval = 60;
	constexpr int kFadeRate = 255;

	constexpr float kTitleScale = 1.0f;

	constexpr float kPressStartScale = 0.35f;
	constexpr int kDecideBlinkFrame = 20;

	//確認バナー幅と高さ
	constexpr int kBannerW = 400;
	constexpr int kBannerH = 150;
	//確認バナー表示位置
	constexpr int kBannerX = Game::kScreenWidth / 2 - kBannerW / 2;
	constexpr int kBannerY = Game::kScreenHeight / 2 - kBannerH / 2;
	constexpr int kConfirmMessageX = kBannerX + 15;
	constexpr int kConfirmMessageY = kBannerY + 20;
	constexpr int kOptionOffsetY = kBannerY + 90;
	constexpr int kOptionYesOffsetX = kBannerX + 70;
	constexpr int kOptionNoOffsetX = kBannerX + 270;

	//選択矢印の位置オフセット
	constexpr int kArrowOffsetX = -20;
	constexpr int kArrowOffsetY = 5;
	constexpr float kArrowScale = 1.0f;

	//画面配置用
	constexpr int kTitleLogoOffsetY = -150;
	constexpr int kPressStartOffsetY = 100;

	constexpr float kPlayerExitOffsetX = 100.0f;
	constexpr float kDemoZombieSpawnOffsetX = 50.0f;

	constexpr float kDemoPlayerStartX = 200.0f;
	constexpr float kDemoPlayerStartY = 510.0f;
	constexpr float kDemoZombieStartY = 500.0f;

	//タイトルロゴの演出用
	constexpr float kTitleBaseScale = 1.5f;
	constexpr float kTitlePulseAmplitude = 0.05f;
	constexpr float kTitlePulseSpeed = 0.05f;

	//スタートボタン点滅
	constexpr int kDecideBlinkDiv = 3;
	constexpr int kBlinkBaseAlpha = 128;
	constexpr int kBlinkAlphaRange = 127;
	constexpr float kBlinkSpeed = 0.05f;

	//確認バナーのアルファ値
	constexpr int kConfirmBgAlpha = 180;

	//矢印調整用
	constexpr int kArrowExtraOffsetY = 5;
	constexpr int kFontSize = 24;
}

void TitleScene::FadeInUpdate(Input& input)
{
	if (--frame_ <= 0)
	{
		update_ = &TitleScene::NormalUpdate;
		draw_ = &TitleScene::NormalDraw;
		return;
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	//フェードアウト中はデモプレイヤーを更新しない
	if (update_ == &TitleScene::FadeOutUpdate)
	{
		return;
	}

	if (demoPlayer_)
	{
		demoPlayer_->Update();
	}

	for (auto& z : demoZombies_)
	{
		z->Update();
	}

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

	if (input.IsTriggered("any_button"))
	{
#ifdef _DEBUG
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		return;
#endif
		Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
		if (isSkipedConfirm_)
		{
			//確認画面をスキップしている場合、直接フェードアウトへ
			nextStage_ = StageType::Stage1;
			frame_ = 0;
			update_ = &TitleScene::FadeOutUpdate;
			draw_ = &TitleScene::FadeDraw;
			return;
		}

		isDeciding_ = true;
		pressBlinkFrame_ = 0;
		decideBlinkCount_ = 0;
	}
}

void TitleScene::FadeOutUpdate(Input& input)
{
	if (frame_++ >= kFadeInterval)
	{
		//プレイ回数カウント
		controller_.GetProgress().playCount_++;

		//プレイヤーをリセットしておく
		demoPlayer_.reset();

		//シーン切り替え
#ifdef _DEBUG
		controller_.ChangeScene(std::make_shared<SelectScene>(controller_));
#else
		controller_.ChangeScene(std::make_shared<GameScene>(controller_, nextStage_));
#endif
		return;
	}
}

void TitleScene::ConfirmUpdate(Input& input)
{
	if (input.IsTriggered("left") || input.IsTriggered("up"))
	{
		if (confirmSelect_ == 0)
		{
			//これ以上左に行けない場合はキャンセル音
			Application::GetInstance().GetSEManager().PlaySE(SE::Cancel);
		}
		else
		{
			confirmSelect_ = 0;
			Application::GetInstance().GetSEManager().PlaySE(SE::Select);
		}
	}
	if (input.IsTriggered("right") || input.IsTriggered("down"))
	{
		if (confirmSelect_ == 1)
		{
			//これ以上右に行けない場合はキャンセル音
			Application::GetInstance().GetSEManager().PlaySE(SE::Cancel);
		}
		else
		{
			confirmSelect_ = 1;
			Application::GetInstance().GetSEManager().PlaySE(SE::Select);
		}
	}

	if (input.IsTriggered("any_button"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
		if (confirmSelect_ == 0)
		{
			//はい：チュートリアルステージへ
			nextStage_ = StageType::Tutorial;
		}
		else
		{
			//いいえ：ステージ1へ
			nextStage_ = StageType::Stage1;
		}

		//デモプレイヤー歩行開始
		if (demoPlayer_)
		{
			demoPlayer_->StartWalk();
		}

		//フェードアウトへ遷移
		frame_ = 0;
		update_ = &TitleScene::PlayerRunUpdate;
		draw_ = &TitleScene::NormalDraw;
	}
}

void TitleScene::PlayerRunUpdate(Input&)
{
	if (demoPlayer_)
	{
		demoPlayer_->Update();

		//画面外に出たらフェードアウト開始
		if (demoPlayer_->GetPos().x > Game::kScreenWidth + kPlayerExitOffsetX)
		{
			frame_ = 0;
			update_ = &TitleScene::FadeOutUpdate;
			draw_ = &TitleScene::FadeDraw;
			Application::GetInstance().GetSEManager().StopSE(SE::PlayerWalk);
		}
	}

	for (auto& z : demoZombies_)
	{
		z->Update();
	}
}

void TitleScene::NormalDraw()
{
	DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, bgH_, false);

	if (pBg_)
	{
		pBg_->DrawTitle();
	}

	for (auto& z : demoZombies_)
	{
		z->Draw();
	}

	//デモプレイヤー描画
	if (demoPlayer_)
	{
		demoPlayer_->Draw();
	}

	float scale = kTitleBaseScale + kTitlePulseAmplitude * sinf(titleRogoPulseFrame_ * kTitlePulseSpeed);  //1.5を中心に+0.05～-0.05で拡大縮小
	titleRogoPulseFrame_++;

	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2 + kTitleLogoOffsetY, scale, 0.0f, titleH_, true);

	//PressStartUIの点滅表示
	if (isDeciding_)
	{
		//決定時の小刻み点滅
		bool visible = ((pressBlinkFrame_ / kDecideBlinkDiv) % 2) == 0;

		if (visible)
		{
			DrawRotaGraph(Game::kScreenWidth / 2,
				Game::kScreenHeight / 2 + kPressStartOffsetY,
				kPressStartScale,
				0.0f,
				pressStartH_,
				true);
		}
	}
	else
	{
		//通常時の点滅
		int alpha = static_cast<int>(kBlinkBaseAlpha + kBlinkAlphaRange * sinf(pressBlinkFrame_ * kBlinkSpeed));

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRotaGraph(
			Game::kScreenWidth / 2,
			Game::kScreenHeight / 2 + kPressStartOffsetY,
			kPressStartScale,
			0.0f,
			pressStartH_,
			true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void TitleScene::FadeDraw()
{
	//値の範囲を一旦0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = 1.0f - static_cast<float>(frame_) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kFadeRate * rate));//αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}

void TitleScene::ConfirmDraw()
{
	//通常のタイトルを描画
	NormalDraw();

	//半透明背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kConfirmBgAlpha);
	DrawBox(kBannerX, kBannerY, kBannerX + kBannerW, kBannerY + kBannerH, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//メッセージ
	DrawStringToHandle(kConfirmMessageX, kConfirmMessageY, "このゲームを初めて遊びますか？", 0xffffff, fontH_);

	// 選択肢
	DrawStringToHandle(kOptionYesOffsetX, kOptionOffsetY, "はい", confirmSelect_ == 0 ? 0xff0000 : 0xffffff, fontH_);
	DrawStringToHandle(kOptionNoOffsetX, kOptionOffsetY, "いいえ", confirmSelect_ == 1 ? 0xff0000 : 0xffffff, fontH_);

	int arrowX = (confirmSelect_ == 0)
		? kOptionYesOffsetX
		: kOptionNoOffsetX;

	int arrowY = kOptionOffsetY;

	DrawRotaGraph(
		arrowX + kArrowOffsetX,
		arrowY + kArrowOffsetY + kArrowExtraOffsetY,
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
	bgH_(-1),
	fontH_(-1),
	frame_(0),
	pressBlinkFrame_(0),
	titleRogoPulseFrame_(0),
	isDeciding_(false),
	decideBlinkCount_(0),
	isSkipedConfirm_(false),
	nextStage_(StageType::Tutorial),
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
	pressStartH_ = LoadGraph("data/UI/PressButton_1.png");
	selectH_ = LoadGraph("data/Bullet/Lance.png");
	bgH_ = LoadGraph("data/map/bg.png");

	fontH_ = CreateFontToHandle("g_コミックホラー悪党-教漢", kFontSize, -1, -1);
	frame_ = kFadeInterval;

	demoZombies_.clear();

	demoZombies_.push_back(std::make_shared<DemoZombie>(Vector2{ Game::kScreenWidth + kDemoZombieSpawnOffsetX, kDemoZombieStartY }));
	demoPlayer_ = std::make_shared<DemoPlayer>(
		Vector2{ kDemoPlayerStartX, kDemoPlayerStartY });
	pBg_ = std::make_shared<Bg>(StageType::Stage1);

	demoPlayer_->Init();
	pBg_->Init();

	for (auto& z : demoZombies_)
	{
		z->Init();
	}

	//タイトルBGM再生
	Application::GetInstance().GetBGMManager().PlayBGM(BGM::Title);

	auto& progress = controller_.GetProgress();

	if (progress.IsReturnFromGame())
	{
		isSkipedConfirm_ = true;

		progress.SetReturnFromGame(false);
	}
}

void TitleScene::Update(Input& input)
{
	(this->*update_)(input);
}

void TitleScene::Draw()
{
	(this->*draw_)();
}