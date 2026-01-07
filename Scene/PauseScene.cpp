#include "PauseScene.h"
#include "Input.h"
#include"TitleScene.h"
#include "SceneController.h"
#include "Application.h"
#include"Dxlib.h"

constexpr int frame_margin = 10;//枠が画面端からどれくらい離れているか
constexpr int appear_interval = 10;//枠が出現するまでのフレーム数
constexpr int menu_row_height = 50;//メニューの行の高さ
constexpr int menu_left_margin = 200;//メニュー枠からの左余白
constexpr int menu_top_margin = 120;//メニュー枠からの上余白

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
	if (input.IsTriggered("next")) 
	{
		ExcecuteMenu();
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
void PauseScene::ExcecuteMenu() 
{
	if (menuList_.empty()) return;
	if (selectIndex_ < 0 || selectIndex_ >= static_cast<int>(menuList_.size())) return;
	const std::string& menu = menuList_[selectIndex_]; 
	if (menu == "ゲームに戻る") {
		update_ = &PauseScene::DisappearUpdate;
		draw_ = &PauseScene::IntervalDraw; 
		frame_ = appear_interval; 
	} 
	else if (menu == "タイトルに戻る") 
	{ 
		controller_.PopScene();
		controller_.ChangeScene(std::make_shared<TitleScene>(controller_));
		return;
	} 
	else if (menu == "ゲームを終了する") 
	{
		Application::GetInstance().RequestExit();
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

PauseScene::PauseScene(SceneController& controller) :
	Scene(controller),
	update_(&PauseScene::AppearUpdate),
	draw_(&PauseScene::IntervalDraw)
{
	menuList_ = {
		"ゲームに戻る",
		"タイトルに戻る",
		"ゲームを終了する"
	};
}

void PauseScene::Init()
{
	frame_ = 0;
	selectIndex_ = 0;

	if (menuList_.empty())
	{
		menuList_ =
		{
			"ゲームに戻る",
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

void PauseScene::Draw() 
{
	(this->*draw_)();
}