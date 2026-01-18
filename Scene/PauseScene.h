#pragma once
#include"Scene.h"
#include<vector>
#include<map>
#include<functional>//ラムダ式を書くために必要
#include<string>

enum class PauseMode
{
	Menu,
	Volume
};

/// <summary>
/// ポーズシーン
/// </summary>
class PauseScene :public Scene
{
private:

	int frame_ = 0;
	int selectIndex_ = 0;//現在選択中のインデックス
	int volumeSelectIdx_ = 0;
	int frameHandle_;
	std::vector<std::string>menuList_;//ポーズ画面中に表示するメニューリスト
	PauseMode pauseMode_ = PauseMode::Menu;

private:
	void AppearUpdate(Input& input);
	void NormalUpdate(Input& input);
	void DisappearUpdate(Input& input);
	void MenuUpdate(Input& input);
	void VolumeUpdate(Input& input);
	using UpdateFunc_t = void(PauseScene::*)(Input& input);
	UpdateFunc_t update_;

	void ExcecuteMenu();

	void IntervalDraw();
	void NormalDraw();
	void MenuDraw();
	void VolumeDraw();
	//描画処理で同じものを多数の場所で使っているのでまとめ的な関数
	void DrawItems(const std::vector<std::string>& items, int selectIdx, int yOffset, const char* guideText);
	using DrawFunc_t = void(PauseScene::*)();
	DrawFunc_t draw_;

public:
	PauseScene(SceneController& controller);
	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
};

