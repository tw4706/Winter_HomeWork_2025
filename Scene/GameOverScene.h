#pragma once
#include "Scene.h"
class GameOverScene :public Scene
{
private:
	int frame_ = 0;

	void FadeInUpdate(Input&);
	void NormelUpdate(Input&input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(GameOverScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormelDraw();
	using DrawFunc_t = void(GameOverScene::*)();
	DrawFunc_t draw_;

public:
	GameOverScene(SceneController& controller);

	void Init()override;
	void Update(Input& input)override;
	void Draw()override;

};

