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

	//エフェクトが空かどうかを判定する関数
	bool IsEmpty() const { return effects_.empty(); }

private:
	std::vector<std::shared_ptr<Effect>> effects_;
	Vector2 cameraOffset_ = { 0.0f,0.0f };
};