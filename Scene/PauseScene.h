#pragma once
#include"Scene.h"
#include<vector>
#include<map>
#include<functional>//ラムダ式を書くために必要
#include<string>

/// <summary>
/// ポーズシーン
/// </summary>
class PauseScene :public Scene
{
private:

	int frame_ = 0;
	int selectIndex_ = 0;//現在選択中のインデックス

	using UpdateFunc_t = void(PauseScene::*)(Input& input);
	UpdateFunc_t update_;
	void AppearUpdate(Input& input);
	void NormalUpdate(Input& input);
	void DisappearUpdate(Input& input);

	void ExcecuteMenu();

	using DrawFunc_t = void(PauseScene::*)();
	DrawFunc_t draw_;
	void IntervalDraw();
	void NormalDraw();
	std::vector<std::string>menuList_;//ポーズ画面中に表示するメニューリスト
	void DrawMenu();

public:
	PauseScene(SceneController& controller);
	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
};

