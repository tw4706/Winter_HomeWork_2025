#include "Camera.h"
#include "GlobalConstants.h"
#include<cmath>

Vector2 VLerp(Vector2 start, Vector2 end, float t)
{
	Vector2 ret;
	ret.x = std::lerp(start.x, end.x, t);
	ret.y = std::lerp(start.y, end.y, t);
	return ret;
}

Camera::Camera():
	pos_{},
	drawOffset_{}
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	pos_ = { 0,0 };
}


void Camera::Update(std::shared_ptr<Player> player)
{
	//プレイヤーの位置とカメラの位置を同じにする
	pos_ = VLerp(pos_, player->GetPos(), 0.05);

	//Draw側に足しているCamera.pos.x/yは反転させる
	drawOffset_.x = pos_.x * -1;
	drawOffset_.y = pos_.y * -1;

	//画面の中央にプレイヤーが来るようにする
	drawOffset_.x =drawOffset_.x + (Game::kScreenWidth * 0.5f);
	drawOffset_.y = drawOffset_.y + (Game::kScreenHeight * 0.5f);
}
