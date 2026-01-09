#pragma once
#include "Effect.h"
#include"Camera.h"
#include <vector>
#include <memory>
class EffectManager
{
public:
	void Update();
	void Draw();
	void AddEffect(std::shared_ptr<Effect> effect);

	//カメラオフセットの設定
	void SetCameraOffset(const Vector2& offset);

private:
	std::vector<std::shared_ptr<Effect>> effects_;
	Vector2 cameraOffset_;
};