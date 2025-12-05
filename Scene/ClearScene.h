#pragma once
#include "Scene.h"
class ClearScene :public Scene
{
private:
	int frame_ = 0;

	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(ClearScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void(ClearScene::*)();
	DrawFunc_t draw_;
public:
	ClearScene(SceneController&controller);

	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
};

