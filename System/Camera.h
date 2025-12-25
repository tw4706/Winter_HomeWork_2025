#pragma once
#include"../Actor/Player.h"
class Camera
{
public:
	Camera();
	~Camera();

	void Init(std::shared_ptr<Player> player);
	void Update(std::shared_ptr<Player>player);

	//カメラのオフセットを取得
	const Vector2& GetOffset() const { return drawOffset_; }

	//カメラを揺らす
	void Shake(int duration, float magnitude);

	bool IsShaking() const { return shakeTimer_ < shakeDuration_; }

private:
	Vector2 pos_;
	Vector2 drawOffset_;
	int shakeDuration_; //揺れの残り時間
	float shakeMagnitude_; //揺れの大きさ
	int shakeTimer_; //揺れのタイマー
};

