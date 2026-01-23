#include "PauseScene.h"
#include "Input.h"
#include"TitleScene.h"
#include"SelectScene.h"
#include "BGMManager.h"
#include "SEManager.h"
#include "SceneController.h"
#include "Application.h"
#include"Dxlib.h"

namespace
{
	constexpr int frame_margin = 10;//枠が画面端からどれくらい離れているか
	constexpr int appear_interval = 10;//枠が出現するまでのフレーム数
	constexpr int menu_row_height = 50;//メニューの行の高さ
	constexpr int menu_left_margin = 200;//メニュー枠からの左余白
	constexpr int menu_top_margin = 120;//メニュー枠からの上余白
	constexpr float pause_frame_scale = 0.8f;

	constexpr int innerLeft = 80;
	constexpr int innerTop = 100;

	//メニューの右下余白
	constexpr int right_margin = 20;
	constexpr int bottom_margin = 20;

	constexpr int offsetX = -150; // 中央より左
	constexpr int offsetY = -30;   // 中央より少し下
}

void PauseScene::AppearUpdate(Input& input)
{
	if (frame_ == appear_interval)
	{
		update_ = &PauseScene::NormalUpdate;
		draw_ = &PauseScene::NormalDraw;
		frame_ = appear_interval;
		return;
	}
	++frame_;
}

void PauseScene::NormalUpdate(Input& input)
{
	if (pauseMode_ == PauseMode::Menu)
	{
		MenuUpdate(input);
	}
	else if (pauseMode_ == PauseMode::Volume)
	{
		VolumeUpdate(input);
	}
}

void PauseScene::DisappearUpdate(Input& input)
{
	if (frame_ == 0)
	{
		controller_.PopScene();
		return;
	}
	--frame_;
}
void PauseScene::MenuUpdate(Input& input)
{
	int menuSize = static_cast<int>(menuList_.size());

	if (input.IsTriggered("up"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Select);
		selectIndex_ = (selectIndex_ + menuSize - 1) % menuSize;
	}

	if (input.IsTriggered("down"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Select);
		selectIndex_ = (selectIndex_ + 1) % menuSize;
	}

	if (input.IsTriggered("next"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
		if (menuList_[selectIndex_] == "設定")
		{
			pauseMode_ = PauseMode::Volume;
			volumeSelectIdx_ = 0; // BGMから
		}
		else
		{
			ExcecuteMenu();
		}
	}
}

void PauseScene::VolumeUpdate(Input& input)
{
	if (input.IsTriggered("up") || input.IsTriggered("down"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Select);
		volumeSelectIdx_ = 1 - volumeSelectIdx_;
	}

	if (input.IsTriggered("left"))
	{
		if (volumeSelectIdx_ == 0)
		{
			auto& bgm = Application::GetInstance().GetBGMManager();
			bgm.SetVolume(bgm.GetVolume() - 5);
		}
		else
		{
			auto& se = Application::GetInstance().GetSEManager();
			se.SetVolume(se.GetVolume() - 5);
		}
	}

	if (input.IsTriggered("right"))
	{
		if (volumeSelectIdx_ == 0)
		{
			auto& bgm = Application::GetInstance().GetBGMManager();
			bgm.SetVolume(bgm.GetVolume() + 5);
		}
		else
		{
			auto& se = Application::GetInstance().GetSEManager();
			se.SetVolume(se.GetVolume() + 5);
		}
	}

	if (input.IsTriggered("next") || input.IsTriggered("pause"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Decide);
		pauseMode_ = PauseMode::Menu;
	}
}

void PauseScene::ExcecuteMenu()
{
	if (menuList_.empty()) return;
	if (selectIndex_ < 0 || selectIndex_ >= static_cast<int>(menuList_.size())) return;
	const std::string& menu = menuList_[selectIndex_];
	if (menu == "ゲームにもどる") {
		update_ = &PauseScene::DisappearUpdate;
		draw_ = &PauseScene::IntervalDraw;
		frame_ = appear_interval;
	}
	else if (menu == "タイトルにもどる")
	{
		//タイトルに戻る処理
		auto& progress = controller_.GetProgress();
		progress.SetReturnFromGame(true);//ゲームシーンから戻ってきたことを記録する
		controller_.GetProgress().Reset();//死亡回数をリセットする

		controller_.ResetScene(std::make_shared<TitleScene>(controller_));
		return;
	}
}

void PauseScene::IntervalDraw()
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	int cx = wsize.w / 2;
	int cy = wsize.h / 2;

	float rate = static_cast<float>(frame_) / appear_interval;

	int imgW, imgH;
	GetGraphSize(frameHandle_, &imgW, &imgH);

	int baseW = static_cast<int>(imgW * pause_frame_scale);
	int baseH = static_cast<int>(imgH * pause_frame_scale);

	int drawW = static_cast<int>(baseW * rate);
	int drawH = static_cast<int>(baseH * rate);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, wsize.w, wsize.h, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawExtendGraph(
		cx - drawW / 2,
		cy - drawH / 2,
		cx + drawW / 2,
		cy + drawH / 2,
		frameHandle_,
		true);
}

void PauseScene::NormalDraw()
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	//黒くて薄いセロファンを張る
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0,wsize.w, wsize.h, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int imgW, imgH;
	GetGraphSize(frameHandle_, &imgW, &imgH);

	int drawW = static_cast<int>(imgW * pause_frame_scale);
	int drawH = static_cast<int>(imgH * pause_frame_scale);

	int x = (wsize.w - drawW) / 2;
	int y = (wsize.h - drawH) / 2;

	DrawExtendGraph(x, y,
		x + drawW, y + drawH,
		frameHandle_, true);

	if (pauseMode_ == PauseMode::Menu)
	{
		MenuDraw();
	}
	else if (pauseMode_ == PauseMode::Volume)
	{
		VolumeDraw();
	}
}

void PauseScene::MenuDraw()
{
	DrawItems(menuList_, selectIndex_, 0, "↑↓で選択 / 決定で進む");
}

void PauseScene::VolumeDraw()
{
	//BGM/SE表示用の文字列
	std::vector<std::string> volItems = {
		"BGM : " + std::to_string(Application::GetInstance().GetBGMManager().GetVolume()),
		"SE  : " + std::to_string(Application::GetInstance().GetSEManager().GetVolume()) };

	DrawItems(volItems, volumeSelectIdx_, 20, "← →で調整 / 決定で戻る"); // 少し下にオフセット
}

void PauseScene::DrawItems(const std::vector<std::string>& items, int selectIdx, int yOffset, const char* guideText)
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	int imgW, imgH;
	GetGraphSize(frameHandle_, &imgW, &imgH);
	int frameW = static_cast<int>(imgW * pause_frame_scale);
	int frameH = static_cast<int>(imgH * pause_frame_scale);
	int frameX = (wsize.w - frameW) / 2;
	int frameY = (wsize.h - frameH) / 2;

	int x = frameX + frameW / 2 + offsetX;
	int y = frameY + frameH / 2 + offsetY + yOffset - static_cast<int>(items.size()) * menu_row_height / 2;

	// 選択カーソルと項目描画
	for (int i = 0; i < items.size(); ++i)
	{
		if (i == selectIdx)
			DrawStringToHandle(x - 30, y + i * menu_row_height, "⇒", 0xffaaaa, fontHandle_);

		uint32_t col = (i == selectIdx) ? GetColor(128, 255, 192) : 0xffffff;
		DrawStringToHandle(x, y + i * menu_row_height, items[i].c_str(), col, fontHandle_);
	}

	// 操作ガイド
	if (guideText)
		DrawStringToHandle(x, y + static_cast<int>(items.size()) * menu_row_height + 5, guideText, 0xaaaaaa, fontHandle_);
}

PauseScene::PauseScene(SceneController& controller) :
	Scene(controller),
	frameHandle_(-1),
	fontHandle_(-1),
	update_(&PauseScene::AppearUpdate),
	draw_(&PauseScene::IntervalDraw)
{
	menuList_ = {
		"ゲームにもどる",
		"設定",
		"タイトルにもどる" };
}

PauseScene::~PauseScene()
{
	DeleteFontToHandle(fontHandle_);
}

void PauseScene::Init()
{
	frame_ = 0;
	selectIndex_ = 0;
	frameHandle_ = LoadGraph("data/UI/Tutorialframe.png");
	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 24, -1, -1);
	pauseMode_ = PauseMode::Menu;

	if (menuList_.empty())
	{
		menuList_ =
		{
			"ゲームにもどる",
			"設定",
			"タイトルにもどる"
		};
	}

	update_ = &PauseScene::AppearUpdate;
	draw_ = &PauseScene::IntervalDraw;
}

void PauseScene::Update(Input& input)
{
	(this->*update_)(input);
}

void PauseScene::Draw()
{
	(this->*draw_)();
}