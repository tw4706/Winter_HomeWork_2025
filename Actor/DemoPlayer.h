#pragma once
#include "GameObject.h"
#include "Animation.h"
#include <memory>

enum class DemoState
{
	Idle,
	Walk
};

class DemoPlayer :public GameObject
{
public:
	DemoPlayer(const Vector2& pos);
	~DemoPlayer();

	void Init() override;
	void Update() override;
	void Draw() override;

	void StartWalk();

private:
	std::shared_ptr<Animation> anim_;
	int graphHandle_;
	int stateTimer_ = 0;
	DemoState state_;
};

