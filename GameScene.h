#pragma once
#include "Scene.h"
#include "Player.h"
#include "Zombie.h"
#include "Dog.h"
#include"Geometry.h"
#include<memory>
class GameScene :public Scene
{
private:

	int frame_ = 0;// フェードインアウト用

	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void (GameScene::*)(Input&);
	UpdateFunc_t update_;//update系を受け取るメンバ関数ポインタ

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (GameScene::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ

public:
	GameScene(SceneController& controller);
	void Init();
	void Update(Input&)override;
	void Draw()override;

private:
	std::shared_ptr<Player>player_;
	std::vector<std::shared_ptr<Enemy>>enemies_;
};