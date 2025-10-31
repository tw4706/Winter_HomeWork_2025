#pragma once
#include "Scene.h"
class TitleScene :public Scene
{
private:
	int frame_;//åoâﬂÉtÉåÅ[ÉÄ

	void FadeInUpdate(Input&);
	void NormalUpdate(Input&input);
	void FadeOutUpdate(Input&input);
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

