#include"GameObject.h"

namespace
{
	constexpr float kGravity = 0.3f;  // 重力
	constexpr float kCharaSize = 32.0f;//キャラクターサイズ
}

GameObject::GameObject() :
	pos_{}, vel_{}, isTurn_(false), 
	isGround_(false),
	graphW_(kCharaSize),
	graphH_(kCharaSize),
	colSize_(kCharaSize), drawOffset_{}
{
}

GameObject::GameObject(Vector2 pos, Vector2 vel):
	pos_{}, vel_{}
{
}

GameObject::GameObject(Vector2 pos, Vector2 vel,
	float graphW, float graphH, float colSize) :
	pos_(pos),
	vel_(vel),
	isTurn_(false),
	isGround_(false),
	graphW_(graphW),
	graphH_(graphH),
	colSize_(colSize),
	drawOffset_{}
{

}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	if (!isGround_)
	{
		Gravity();
	}
	Rect chipRect;
	CheckHitMap(chipRect);
}

void GameObject::Draw()
{
#ifdef _DEBUG
	//当たり判定を表示
	colRect_.Draw(0x0000ff, false);
#endif

}

void GameObject::Gravity() 
{
	vel_.y += kGravity;
}

void GameObject::CheckHitMap(Rect& chipRect) {
	// X方向の移動
	pos_.x += vel_.x;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	if (pBg_->IsCollision(colRect_, chipRect)) {

		if (vel_.x > 0.0f) {

			pos_.x = chipRect.GetLeft() - (colSize_ / 2.0f); // 右にめり込み防止
		}
		else if (vel_.x < 0.0f)
		{
			pos_.x = chipRect.GetRight() + (colSize_ / 2.0f); // 左にめり込み防止
		}
		vel_.x = 0.0f; // 横方向の速度を止める
	}

	// Y方向の移動
	pos_.y += vel_.y;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	if (pBg_->IsCollision(colRect_, chipRect)) {
		if (vel_.y > 0.0f) {
			pos_.y = chipRect.GetTop() - (colSize_ / 2.0f); // 地面に乗る
			isGround_ = true;
		}
		else if (vel_.y < 0.0f) {
			pos_.y = chipRect.GetBottom() + (colSize_ / 2.0f); // 天井に当たる
		}
		vel_.y = 0.0f; // 縦方向の速度を止める（跳ね返りなし）
	}
}