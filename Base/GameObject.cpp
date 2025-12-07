#include"GameObject.h"

namespace
{
	constexpr float kGravity = 0.5f;  // 重力
	constexpr float kCharaSize = 32.0f;//キャラクターサイズ
}

GameObject::GameObject() :
	pos_{}, vel_{}, isTurn_(false), 
	isGround_(false),
	graphW_(kCharaSize),
	graphH_(kCharaSize),
	colSize_(kCharaSize), cameraOffset_{}
{
}

GameObject::GameObject(Vector2 pos, Vector2 vel):
	pos_(pos),
	vel_(vel),
	isTurn_(false),
	isGround_(false),
	graphW_(kCharaSize),
	graphH_(kCharaSize),
	colSize_(kCharaSize),
	cameraOffset_{}
{
}

GameObject::GameObject(Vector2 pos, Vector2 vel,
	float graphW, float graphH, float colSize) :
	pos_(pos),
	vel_(vel),
	isTurn_(false),
	isGround_(false),
	chipRect_(),
	graphW_(graphW),
	graphH_(graphH),
	colSize_(colSize),
	cameraOffset_{}
{

}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	Gravity();

	CheckMapCollision(chipRect_);
	if (!isGround_)
	{
		isGround_ = IsOnGround();
	}
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
	//重力が無効なら処理しない
	if (!useGravity_) return;

	//重力を加算
	vel_.y += kGravity;
}

void GameObject::CheckMapCollision(Rect& chipRect) 
{
	//X方向の移動
	pos_.x += vel_.x;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_-1, colSize_-1);

	if (pBg_->IsCollision(colRect_, chipRect)) {

		if (vel_.x >= 0.0f) 
		{

			pos_.x = chipRect.GetLeft() - (colSize_ / 2.0f); //右にめり込み防止
		}
		else if (vel_.x <= 0.0f)
		{
			pos_.x = chipRect.GetRight() + (colSize_ / 2.0f); //左にめり込み防止
		}
		vel_.x = 0.0f; //横方向の速度を止める
	}

	//Y方向の移動
	pos_.y += vel_.y;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_-1, colSize_-1);

	if (pBg_->IsCollision(colRect_, chipRect)) 
	{
		if (vel_.y >= 0.0f) 
		{
			pos_.y = chipRect.GetTop() - (colSize_ / 2.0f); //地面に乗る
			isGround_ = true;
		}
		else if (vel_.y <= 0.0f) 
		{
			pos_.y = chipRect.GetBottom() + (colSize_ / 2.0f); //天井に当たる
		}
		vel_.y = 0.0f; //縦方向の速度を止める
	}
}

bool GameObject::IsOnGround()
{
	//足元に少し下の矩形を作る
	Rect footRect = colRect_;
	//下方向にオフセット
	footRect.top_ += 4.0f;    
	footRect.bottom_ += 4.0f;

	return pBg_->IsCollision(footRect, chipRect_);
}

void GameObject::CheckTorchAndMapCollision()
{
	isGround_ = false;

	// まず垂直方向に移動
	pos_.y += vel_.y;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	// 足元だけ判定する
	Rect foot = colRect_;
	foot.top_ += 2;
	foot.bottom_ += 2;

	Rect nextRect;

	if (pBg_->IsCollision(foot, chipRect_))
	{
		// 地面の上に乗る
		pos_.y = chipRect_.GetTop() - colSize_ / 2.0f;
		vel_.y = 0;
		isGround_ = true;
	}
}

void GameObject::SetUseGravity(bool enable)
{
	useGravity_ = enable;
}
