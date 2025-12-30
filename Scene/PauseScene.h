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
	//メニュー名と実行内容の対応テーブル
	std::map<std::string, std::function<void(Input&)>>execTable_;

	int frame_ = 0;
	int selectIndex_ = 0;//現在選択中のインデックス

	int yesNoIndex = 1;//Noがデフォルトですが、YES=0,NO=1とします。
	//YES?NOの後で実行してほしい内容
	std::function<void()>yesRequestFunction_ = []() {};

	using UpdateFunc_t = void(PauseScene::*)(Input& input);
	UpdateFunc_t update_;
	void AppearUpdate(Input& input);
	void NormalUpdate(Input& input);
	void DisappearUpdate(Input& input);

	void YesNoDialogUpdate(Input& input);

	using DrawFunc_t = void(PauseScene::*)();
	DrawFunc_t draw_;
	void IntervalDraw();
	void NormalDraw();
	std::vector<std::string>menuList_;//ポーズ画面中に表示するメニューリスト
	void DrawMenu();

	void YesNoDialogDraw();

public:
	PauseScene(SceneController& controller);
	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
};

