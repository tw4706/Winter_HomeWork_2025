#pragma once
#include "Scene.h"
#include "StageType.h"
#include<memory>

class Animation;
class GameOverScene :public Scene
{
public:
	GameOverScene(SceneController& controller, StageType stage);

	void Init()override;
	void Update(Input& input)override;
	void Draw()override;

private:
	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(GameOverScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void(GameOverScene::*)();
	DrawFunc_t draw_;
private:
	int frame_ = 0;
	StageType stageType_;
	int bgHandle_;
	int frameHandle_;
	int deadCircleHandle_;
	int playerDeadGraphHandle_;
	std::shared_ptr<Animation>pAnimation_;
};

