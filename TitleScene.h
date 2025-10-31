#pragma once
#include "Scene.h"
class TitleScene :public Scene
{
private:
	int titleH_;//タイトル画像ハンドル
	int frame_;//経過フレーム

	void FadeInUpdate(Input&);
	void NormalUpdate(Input&);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void (TitleScene::*)(Input&);
	UpdateFunc_t update_;

	void NormalDraw();
	void FadeDraw();
	using DrawFunc_t = void(TitleScene::*)();
	DrawFunc_t draw_;

public:
	TitleScene(SceneController&);
	void Update(Input&)override;
	void Draw()override;
};

