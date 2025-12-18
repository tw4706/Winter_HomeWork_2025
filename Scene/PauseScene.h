#pragma once
#include "Scene.h"
#include <vector>
#include <string>
#include<map>
#include<functional>


class PauseScene :public Scene
{
private:

	//メニュー名と実行内容の対応テーブル
	std::map<std::string, std::function<void(Input&)>>menuActions_;

	int frame_ = 0;
	int selectIdx_ = 0;

	//はい・いいえダイアログの選択中インデックス
	int yesNoDialogSelectIdx_ = 1;

	//はい・いいえあとに実行する内容
	std::function<void()>requestFunction_ = []() {};

	using UpdateFunc_t = void(PauseScene::*)(Input& input);
	UpdateFunc_t update_;
	//ポーズ画面の表示中の更新
	void AppearUpdate(Input& input);
	//ポーズ画面の通常更新
	void NormalUpdate(Input& input);
	//ポーズ画面の消失中の更新
	void DisappearUpdate(Input& input);
	//はい・いいえダイアログ表示中の更新
	void YesNoDialogUpdate(Input& input);


	using DrawFunc_t = void(PauseScene::*)();
	DrawFunc_t draw_;
	//ポーズ画面の表示開始時の描画
	void IntervalDraw();
	//ポーズ画面の通常描画
	void NormalDraw();

	//ポーズ画面中に表示するメニューリスト
	std::vector<std::string>menuList_;
	//メニューの選択中インデックスの描画
	void DrawMenu();
	void YesNoDialogDraw();


public:
	PauseScene(SceneController& controller);

	void Init() override {};
	void Update(Input& input) override;
	void Draw() override;
};

