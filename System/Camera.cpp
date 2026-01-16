#include "Camera.h"
#include "../GlobalConstants.h"
#include<cmath>

namespace
{
	constexpr float kMapWidth = 9000.0f;//マップ全体の幅
	constexpr float kMapHeight = 1080.0f;//マップ全体の高さ
}

Vector2 VLerp(Vector2 start, Vector2 end, float t)
{
	Vector2 ret;
	ret.x = std::lerp(start.x, end.x, t);
	ret.y = std::lerp(start.y, end.y, t);
	return ret;
}

Camera::Camera() :
	pos_{},
	drawOffset_{},
	shakeDuration_(0),
	shakeMagnitude_(0.0f),
	shakeTimer_(0)
{
}

Camera::~Camera()
{
}

void Camera::Init(std::shared_ptr<Player> player)
{
	pos_ = player->GetPos();

	drawOffset_.x = -pos_.x + (Game::kScreenWidth * 0.5f);
	drawOffset_.y = -pos_.y + (Game::kScreenHeight * 0.5f);
}

void Camera::Update(std::shared_ptr<Player> player)
{
	// X軸のみプレイヤーを追従
	pos_.x = std::lerp(pos_.x, player->GetPos().x, 0.05f);



	// --- X軸のカメラ制限（左端で止める） ---
	float halfW = Game::kScreenWidth * 0.5f;
	if (pos_.x < halfW) pos_.x = halfW;

	// --- 右端で止める ---
	float rightLimit = kMapWidth - halfW;
	if (pos_.x > rightLimit) pos_.x = rightLimit;

	//Draw側に足しているCamera.pos.x/yは反転させる
	drawOffset_.x = pos_.x * -1;

	//画面の中央にプレイヤーが来るようにする
	drawOffset_.x = drawOffset_.x + (Game::kScreenWidth * 0.5f);
	drawOffset_.y = -pos_.y + (Game::kScreenHeight-200);

	if (shakeTimer_ < shakeDuration_)
	{
		shakeTimer_++;

		float offsetX = ((rand() % 200) / 100.0f - 1.0f) * shakeMagnitude_;
		float offsetY = ((rand() % 200) / 100.0f - 1.0f) * shakeMagnitude_;

		drawOffset_.x += offsetX;
		drawOffset_.y += offsetY;
	}
}

void Camera::Shake(int duration, float magnitude)
{
	shakeDuration_ = duration;
	shakeMagnitude_ = magnitude;
	shakeTimer_ = 0;
}
