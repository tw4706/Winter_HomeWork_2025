#pragma once
#include "Rect.h"
#include"Camera.h"
class Effect
{
public:
	virtual ~Effect() {}
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsDead() const { return isDead_; }


	void SetCameraOffset(const Vector2& offset){cameraOffset_ = offset;}

protected:
	Vector2 pos_;
	Vector2 cameraOffset_;
	int frame_ = 0;
	bool isDead_ = false;
};

