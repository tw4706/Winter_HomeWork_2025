#pragma once
#include "Scene.h"
class SelectScene :public Scene
{
private:
	int frame_;
	int selectIndex_;

	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(SelectScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void(SelectScene::*)();
	DrawFunc_t draw_;
public:
	SelectScene(SceneController& controller);

	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
};

