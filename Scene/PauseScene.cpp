#include "PauseScene.h"
#include "TitleScene.h"
#include"Input.h"
#include "SceneController.h"
#include "Application.h"
#include<Dxlib.h>

namespace
{
	constexpr int kFrameInterval = 10;//枠が画面端からどれくらい離れているか
	constexpr int kAppearInterval = 10;//枠が出現するまでのフレーム数
	constexpr int kMenuRowHeight = 50;//メニューの行の高さ
	constexpr int kMenuLeftInterval = 200;//メニュー枠からの左余白
	constexpr int kMenuTopInterval = 120;//メニュー枠からの上余白
	constexpr int kYesDialog = 0;
	constexpr int kNoDialog = 1;
}

void PauseScene::AppearUpdate(Input& input)
{
	if (++frame_ >= kAppearInterval)
	{
		update_ = &PauseScene::NormalUpdate;
		draw_ = &PauseScene::NormalDraw;
	}
}

void PauseScene::NormalUpdate(Input& input)
{
	if (menuList_.empty()) return;

	if (input.IsTriggered("pause"))
	{
		update_ = &PauseScene::DisappearUpdate;
		draw_ = &PauseScene::IntervalDraw;
		frame_ = kAppearInterval;
		return;
	}

	if (input.IsTriggered("up"))
		selectIdx_ = (selectIdx_ + menuList_.size() - 1) % menuList_.size();
	if (input.IsTriggered("down"))
		selectIdx_ = (selectIdx_ + 1) % menuList_.size();

	if (input.IsTriggered("ok"))
	{
		if (selectIdx_ < menuList_.size())
		{
			const auto& name = menuList_[selectIdx_];
			if (menuActions_.count(name))
				menuActions_[name](input);
		}
	}
}

void PauseScene::DisappearUpdate(Input& input)
{
	if (--frame_ <= 0)
	{
		controller_.PopScene();
	}
}

void PauseScene::YesNoDialogUpdate(Input& input)
{
	if (input.IsTriggered("left") || input.IsTriggered("right"))
		yesNoDialogSelectIdx_ = (yesNoDialogSelectIdx_ + 1) % 2;

	if (input.IsTriggered("ok"))
	{
		if (yesNoDialogSelectIdx_ == kYesDialog)
		{
			requestFunction_();
		}
		else
		{
			requestFunction_ = []() {};
		}

		update_ = &PauseScene::NormalUpdate;
		draw_ = &PauseScene::NormalDraw;
	}
}

void PauseScene::IntervalDraw()
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	int center_y = wsize.h / 2;//画面の真ん中のY座標
	int center_x = wsize.w / 2;//画面の真ん中のY座標
	float rate = static_cast<float>(frame_) /
		static_cast<float>(kAppearInterval);//表示割合

	int frame_height = (wsize.h - kFrameInterval) - center_y;//最終的になポーズ枠の高さ
	int frame_width = (wsize.w - kFrameInterval) - center_x;//最終的になポーズ枠の高さ
	frame_height *= rate;
	frame_width *= rate;

	//黒くて薄いセロファンを張る
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(center_x - frame_width,//左 
		center_y - frame_height,//上(10,10)
		center_x + frame_width,//右
		center_y + frame_height, //下
		0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//外枠
	DrawBox(center_x - frame_width,//左
		center_y - frame_height,//上
		center_x + frame_width,//右
		center_y + frame_height,//下
		0xaaaaff, false, 3);
}

void PauseScene::NormalDraw()
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	//黒くて薄いセロファンを張る
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(kFrameInterval, kFrameInterval,//左上(10,10)
		wsize.w - kFrameInterval, wsize.h - kFrameInterval, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//外枠
	DrawBox(kFrameInterval, kFrameInterval,
		wsize.w - kFrameInterval, wsize.h - kFrameInterval,
		0xaaaaff, false, 3);

	//ポーズシーンの文字列表示
	DrawString(280, kFrameInterval + 10, "Pause Scene", 0xffffff);
	DrawMenu();
}

void PauseScene::DrawMenu()
{
	if (menuList_.empty()) return;

	if (selectIdx_ >= menuList_.size()) selectIdx_ = 0;

	int menuStartX = kFrameInterval + kMenuLeftInterval;
	int indicatorX = menuStartX - 30;
	int menuY = kFrameInterval + kMenuTopInterval;
	for (int idx = 0; idx < menuList_.size(); ++idx)
	{
		int offsetX = 0;
		uint32_t col = 0xffffff;
		if (idx == selectIdx_)
		{
			DrawString(indicatorX, menuY, "⇒", 0xffaaaa);
			offsetX = 10;
			col = GetColor(128, 255, 192);
		}
		DrawFormatString(menuStartX + offsetX, menuY,col,
			"%s", menuList_[idx].c_str());

		menuY += kMenuRowHeight;
	}
}

void PauseScene::YesNoDialogDraw()
{
	if (yesNoDialogTitle_.empty()) return;

	//NormalDrawを書いているのは通常のメニューの上に
	//YesNoDialogを置きたいので、メニューの表示をするため
	NormalDraw();

	constexpr int kDialogHeight = 100;
	constexpr int kDialogWidth = 300;
	const int centerY = Application::GetInstance().GetWindowSize().h / 2;
	const int centerX = Application::GetInstance().GetWindowSize().w / 2;

	//YES/NOの枠表示
	const int dialog_left = centerX - kDialogWidth / 2;
	DrawBox(dialog_left,
		centerY - kDialogHeight / 2,
		centerX + kDialogWidth / 2,
		centerY + kDialogHeight / 2,
		0xaa88bb, true);
	DrawBox(dialog_left,
		centerY - kDialogHeight / 2,
		centerX + kDialogWidth / 2,
		centerY + kDialogHeight / 2,
		0xffffff, false, 3);

	int y = centerY - 10;//画面中心から文字サイズの半分引く
	int x = dialog_left + 90;//はい、いいえが真ん中に来るように
	std::array<std::string, 2>answers = { "はい","いいえ" };

	if (selectIdx_ >= menuList_.size()) selectIdx_ = 0;

	//ダイアログタイトルを表示
	DrawFormatString(centerX - 80, centerY - kDialogWidth / 2 + 30,
		0xffffff, "%s", yesNoDialogTitle_.c_str());

	//はい、いいえを表示
	for (int idx = 0; idx < 2; ++idx)
	{
		uint32_t col = 0xffffff;
		if (yesNoDialogSelectIdx_ == idx) {
			DrawString(x - 20, y, "⇒", 0xaaffaa);
			col = GetColor(255, 64, 64);
		}
		DrawFormatString(x, y, col, "%s", answers[idx].c_str());
		x += 100;
	}
}

PauseScene::PauseScene(SceneController& controller): 
	Scene(controller),
	frame_(0),
	selectIdx_(0),
	yesNoDialogSelectIdx_(1),
	update_(&PauseScene::AppearUpdate),
	draw_(&PauseScene::IntervalDraw)

{
	menuList_ = { "ゲームに戻る", "タイトルに戻る", "ゲームを終了する" };

	menuActions_["ゲームに戻る"] = [this](Input&)
		{
			update_ = &PauseScene::DisappearUpdate;
			draw_ = &PauseScene::IntervalDraw;
			frame_ = kAppearInterval;
		};
	menuActions_["タイトルに戻る"] = [this](Input&)
		{
			update_ = &PauseScene::YesNoDialogUpdate;
			draw_ = &PauseScene::YesNoDialogDraw;
			yesNoDialogSelectIdx_ = kNoDialog;
			yesNoDialogTitle_ = "タイトルに戻りますか？";
			requestFunction_ = [this]()
				{
					controller_.ResetScene(std::make_shared<TitleScene>(controller_));
				};
		};
	menuActions_["ゲームを終了する"] = [this](Input&)
		{
			update_ = &PauseScene::YesNoDialogUpdate;
			draw_ = &PauseScene::YesNoDialogDraw;
			yesNoDialogSelectIdx_ = kNoDialog;
			yesNoDialogTitle_ = "ゲームを終了しますか？";
			requestFunction_ = []() { Application::GetInstance().RequestExit(); };
		};
}

void PauseScene::Update(Input& input)
{
	(this->*update_)(input);
}

void PauseScene::Draw()
{
	(this->*draw_)();
}
