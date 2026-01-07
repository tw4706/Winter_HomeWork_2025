#pragma once
#include "Scene.h"
#include "StageType.h"


class GameOverScene :public Scene
{
private:
	int frame_ = 0;
	StageType stageType_;

	void FadeInUpdate(Input&);
	void NormalUpdate(Input&input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(GameOverScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void(GameOverScene::*)();
	DrawFunc_t draw_;

public:
	GameOverScene(SceneController& controller, StageType stage);

	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
};

