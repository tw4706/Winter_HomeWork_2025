#pragma once
#include "Scene.h"
#include <vector>
#include <string>


class PauseScene :public Scene
{
public:
	PauseScene(SceneController&controller);

	void Update(Input&input) override;
	void Draw() override;
private:

	int frame_ = 0;

	//ポーズ画面の状態
	int selectIdx_ = 0;

	//はい・いいえダイアログの選択中インデックス
	int yesNoDialogSelectIdx_ = 0;

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
	std::vector<std::wstring>menuList_;

	//メニューの選択中インデックスの描画
	void DrawMenu();
};

