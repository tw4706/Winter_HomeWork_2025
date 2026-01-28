#include "Camera.h"
#include "../GlobalConstants.h"
#include<cmath>

namespace
{
	constexpr float kMapWidth = 9000.0f;//マップ全体の幅
	constexpr float kMapHeight = 1080.0f;//マップ全体の高さ
	
	//========================
	// カメラ関連
	//========================
	//カメラ挙動
	constexpr float kFollowLerpRate = 0.05f; //プレイヤー追従する速度
	constexpr float kCameraYOffset = 200.0f; //カメラのY座標オフセット


	// カメラシェイク
	constexpr int   kShakeRandRange = 200;
	constexpr float kShakeRandDiv = 100.0f;
	constexpr float kShakeRandBase = 1.0f;
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

	drawOffset_.x = -pos_.x + (Game::kScreenWidth / 2);
	drawOffset_.y = -pos_.y + (Game::kScreenHeight / 2);
}

void Camera::Update(std::shared_ptr<Player> player)
{
	// X軸のみプレイヤーを追従
	pos_.x = std::lerp(pos_.x, player->GetPos().x, kFollowLerpRate);



	// --- X軸のカメラ制限（左端で止める） ---
	float halfW = Game::kScreenWidth / 2;
	if (pos_.x < halfW) pos_.x = halfW;

	// --- 右端で止める ---
	float rightLimit = kMapWidth - halfW;
	if (pos_.x > rightLimit) pos_.x = rightLimit;

	//Draw側に足しているCamera.pos.x/yは反転させる
	drawOffset_.x = pos_.x * -1;

	//画面の中央にプレイヤーが来るようにする
	drawOffset_.x = drawOffset_.x + (Game::kScreenWidth / 2);
	drawOffset_.y = -pos_.y + (Game::kScreenHeight - kCameraYOffset);

	if (shakeTimer_ < shakeDuration_)
	{
		shakeTimer_++;

		float offsetX = ((rand() % kShakeRandRange) / kShakeRandDiv - kShakeRandBase) * shakeMagnitude_;
		float offsetY = ((rand() % kShakeRandRange) / kShakeRandDiv - kShakeRandBase) * shakeMagnitude_;

		drawOffset_.x += offsetX;
		drawOffset_.y += offsetY;
	}
}

Vector2 Camera::GetLeftTop() const
{
	return Vector2{
	pos_.x - Game::kScreenWidth / 2,
	pos_.y - Game::kScreenHeight / 2 };
}

Vector2 Camera::GetRightBottom() const
{
	return Vector2{
	pos_.x + Game::kScreenWidth / 2,
	pos_.y + Game::kScreenHeight / 2 };
}

void Camera::Shake(int duration, float magnitude)
{
	shakeDuration_ = duration;
	shakeMagnitude_ = magnitude;
	shakeTimer_ = 0;
}
