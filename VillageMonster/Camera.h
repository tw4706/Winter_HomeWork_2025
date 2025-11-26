#pragma once
#include"Player.h"
class Camera
{
public:
	Camera();
	~Camera();

	void Init();
	void Update(std::shared_ptr<Player>player);

	//カメラのオフセットを取得
	const Vector2& GetOffset() const { return drawOffset_; }

private:
	Vector2 pos_;
	Vector2 drawOffset_;
};

