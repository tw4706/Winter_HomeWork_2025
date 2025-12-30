#include "PauseScene.h"
#include "Input.h"
#include "SceneController.h"
#include "Application.h"
#include "TitleScene.h"
#include"Dxlib.h"

constexpr int frame_margin = 10;//枠が画面端からどれくらい離れているか
constexpr int appear_interval = 10;//枠が出現するまでのフレーム数
constexpr int menu_row_height = 50;//メニューの行の高さ
constexpr int menu_left_margin = 200;//メニュー枠からの左余白
constexpr int menu_top_margin = 120;//メニュー枠からの上余白
constexpr int yes_no_dialog_yes = 0;
constexpr int yes_no_dialog_no = 1;

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
	if (menuList_.empty())return;
	//最初の枠出現アニメーション中は何もしない
	if (frame_ < appear_interval) return;

	if (input.IsTriggered("pause"))
	{
		update_ = &PauseScene::DisappearUpdate;
		draw_ = &PauseScene::IntervalDraw;
		frame_ = appear_interval;
		return;
	}
	if (input.IsTriggered("up")) {
		selectIndex_ = (selectIndex_ + menuList_.size() - 1) % menuList_.size();
	}
	if (input.IsTriggered("down")) {
		selectIndex_ = (selectIndex_ + 1) % menuList_.size();
	}
	if (input.IsTriggered("ok")) {
		//現在選択中のメニューアイテム名取得する
		auto& menuName = menuList_[selectIndex_];
		//その目ny－アイテムの名前に対応付けられたラムダ式を実行する
		execTable_[menuName](input);
		return;
	}
}

void PauseScene::DisappearUpdate(Input& input)
{
	if (frame_ == 0)
	{
		controller_.PopScene();//この時点で自分は解放されている
		return;
	}
	--frame_;
}

void PauseScene::YesNoDialogUpdate(Input& input)
{
	if (input.IsTriggered("left") || input.IsTriggered("right"))
	{
		yesNoIndex = (yesNoIndex + 1) % 2;
		return;
	}
	if (input.IsTriggered("ok"))
	{
		//YESが選ばれた
		if (yesNoIndex == yes_no_dialog_yes)
		{
			yesRequestFunction_();
		}
		else//NOが選ばれた
		{
			yesRequestFunction_ = []() {};//念のためクリア
			update_ = &PauseScene::NormalUpdate;
			draw_ = &PauseScene::NormalDraw;
		}
		return;
	}
}

void PauseScene::IntervalDraw()
{
	const auto& wsize = Application::GetInstance().GetWindowSize();

	int center_y = wsize.h / 2;//画面の真ん中のY座標
	int center_x = wsize.w / 2;//画面の真ん中のY座標
	float rate = static_cast<float>(frame_) /
		static_cast<float>(appear_interval);//表示割合

	int frame_height = (wsize.h - frame_margin) - center_y;//最終的になポーズ枠の高さ
	int frame_width = (wsize.w - frame_margin) - center_x;//最終的になポーズ枠の高さ
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
	DrawBox(frame_margin, frame_margin,//左上(10,10)
		wsize.w - frame_margin, wsize.h - frame_margin, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//外枠
	DrawBox(frame_margin, frame_margin,
		wsize.w - frame_margin, wsize.h - frame_margin,
		0xaaaaff, false, 3);

	//ポーズシーンの文字列表示
	DrawString(280, frame_margin + 10, "Pause Scene", 0xffffff);
	DrawMenu();
}

void PauseScene::DrawMenu()
{

	int menuStartX = frame_margin + menu_left_margin;
	int indicatorX = menuStartX - 30;
	int menuY = frame_margin + menu_top_margin;
	for (int idx = 0; idx < menuList_.size(); ++idx)
	{
		int offsetX = 0;
		uint32_t col = 0xffffff;
		if (idx == selectIndex_)
		{
			DrawString(indicatorX, menuY, "⇒", 0xffaaaa);
			offsetX = 10;
			col = GetColor(128, 255, 192);
		}
		DrawFormatString(menuStartX + offsetX, menuY,
			col,
			"%s", menuList_[idx].c_str());

		menuY += menu_row_height;
	}
}

void PauseScene::YesNoDialogDraw()
{
	if (menuList_.empty() || selectIndex_ >= menuList_.size())
	{
		return;
	}

	//NormalDrawを書いているのは通常のメニューの上に
	//YesNoDialogを置きたいので、メニューの表示をするため
	NormalDraw();

	constexpr int yes_no_dialog_height = 100;
	constexpr int yes_no_dialog_width = 300;
	const int centerY = Application::GetInstance().GetWindowSize().h / 2;
	const int centerX = Application::GetInstance().GetWindowSize().w / 2;

	//YES/NOの枠表示
	const int dialog_left = centerX - yes_no_dialog_width / 2;
	DrawBox(dialog_left,
		centerY - yes_no_dialog_height / 2,
		centerX + yes_no_dialog_width / 2,
		centerY + yes_no_dialog_height / 2,
		0xaa88bb, true);
	DrawBox(dialog_left,
		centerY - yes_no_dialog_height / 2,
		centerX + yes_no_dialog_width / 2,
		centerY + yes_no_dialog_height / 2,
		0xffffff, false, 3);

	int y = centerY - 10;//画面中心から文字サイズの半分引く
	int x = dialog_left + 90;//はい、いいえが真ん中に来るように
	std::array<std::wstring, 2>answers = { L"はい",L"いいえ" };

	//ダイアログタイトルを表示
	DrawFormatString(centerX - 80, centerX - yes_no_dialog_width / 2 + 30,
		0xffffff, "%s", menuList_[selectIndex_].c_str());

	//はい、いいえを表示
	for (int idx = 0; idx < 2; ++idx)
	{
		uint32_t col = 0xffffff;
		if (yesNoIndex == idx) {
			DrawString(x - 20, y, "⇒", 0xaaffaa);
			col = GetColor(255, 64, 64);
		}
		DrawFormatString(x, y, col, "%s", answers[idx].c_str());
		x += 100;
	}
}

PauseScene::PauseScene(SceneController& controller) :
	Scene(controller),
	update_(&PauseScene::AppearUpdate),
	draw_(&PauseScene::IntervalDraw)
{
	menuList_ = {
		"ゲームに戻る",
		"キーコンフィグ",
		"タイトルに戻る",
		"ゲームを終了する"
	};
	//メニューで選ばれる文字列と実行される内容のペアを定義S
	execTable_["ゲームに戻る"] = [this](Input&)
		{
			update_ = &PauseScene::DisappearUpdate;
			draw_ = &PauseScene::IntervalDraw;
			frame_ = appear_interval;
			return;
		};
	execTable_["キーコンフィグ"] = [this](Input& input)
		{
			controller_.PushScene(std::make_shared<TitleScene>(controller_));
		};
	execTable_["タイトルに戻る"] = [this](Input&)
		{
			update_ = &PauseScene::YesNoDialogUpdate;
			draw_ = &PauseScene::YesNoDialogDraw;
			yesRequestFunction_ = [this]()
				{
					controller_.ResetScene(std::make_shared<TitleScene>(controller_));
				};
		};
	execTable_["ゲームを終了する"] = [this](Input&)
		{
			update_ = &PauseScene::YesNoDialogUpdate;
			draw_ = &PauseScene::YesNoDialogDraw;
			yesRequestFunction_ = []()
				{
					Application::GetInstance().RequestExit();
				};
		};
}

void PauseScene::Init()
{
	frame_ = 0;
	selectIndex_ = 0;
	yesNoIndex = 1;

	if (menuList_.empty())
	{
		menuList_ =
		{
			"ゲームに戻る",
			"キーコンフィグ",
			"タイトルに戻る",
			"ゲームを終了する"
		};
	}

	update_ = &PauseScene::AppearUpdate;
	draw_ = &PauseScene::IntervalDraw;
}

void PauseScene::Update(Input& input)
{
	(this->*update_)(input);
}

void PauseScene::Draw() {
	(this->*draw_)();
}