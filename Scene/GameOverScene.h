#pragma once
#include "Scene.h"
#include "StageType.h"
#include <string>
#include <vector>
#include<memory>

class Animation;
class EffectManager;
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
	int bgHandle_;
	int frameHandle_;
	int fontHandle_;
	int fontOptionHandle_;
	int selectHandle_;
	int deadCircleHandle_;
	int playerDeadGraphHandle_;
	int selectIdx_;
	bool isSelecting_;
	int currentTextIdx_;//現在の表示している文字
	bool isTextEffectPlaying_;//エフェクトを再生してるかどうか

	StageType stageType_;
	std::string gameOverText_;
	std::vector<bool>charVisible_;
	std::shared_ptr<Animation>pAnimation_;
	std::shared_ptr<EffectManager> pEffectManager_;
};

