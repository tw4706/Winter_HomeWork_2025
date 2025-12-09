#pragma once
#include "Scene.h"
#include<vector>
#include<string>

class SelectScene :public Scene
{
public:
	SelectScene(SceneController& controller);

	void Init()override;
	void Update(Input& input)override;
	void Draw()override;

private:
	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(SelectScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void(SelectScene::*)();
	DrawFunc_t draw_;

	int frame_;
	int selectIndex_;
	std::vector<std::string>stageMenu_;
};

