#pragma once
#include "Scene.h"
#include <string>
#include <vector>
#include <memory>

class EffectManager;
class ClearScene :public Scene
{
private:
	int frame_ = 0;
	int bgHandle_;
	int fontHandle_;
	int fontTitleHandle_;
	int deathCount_ = 0;

	std::string clearText_;             //GAMECLEARの文字列
	std::vector<bool> charVisible_;     //各文字の表示フラグ

	int currentTextIdx_;
	bool isTextEffectPlaying_;

	std::shared_ptr<EffectManager> pEffectManager_;//エフェクトマネージャーのポインタ

private:
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

