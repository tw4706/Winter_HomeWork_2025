#include"GameObject.h"
#include"Bg.h"

namespace
{
	constexpr float kGravity = 0.3f;  // 重力
	constexpr float kCharaSize = 32.0f;//キャラクターサイズ
}

GameObject::GameObject(Vector2 pos,Vector2 vel):
	pos_(pos),
	vel_(vel),
	isTurn_(false),
	isGround_(false),
	pBg_(nullptr)
{

}

GameObject::~GameObject()
{
}

void GameObject::Init()
{

}

void GameObject::Update()
{
	Gravity();
	colRect_.SetCenter(pos_.x, pos_.y, kCharaSize, kCharaSize);
}

void GameObject::Draw()
{
	float drawX = pos_.x - pBg_->GetScrollX() - kCharaSize * 0.5f;
	float drawY = pos_.y - pBg_->GetScrollY() - kCharaSize * 0.5f;
#ifdef _DEBUG
	//当たり判定を表示
	colRect_.Draw(0x0000ff, false);
#endif

}

void GameObject::Gravity() {
	vel_.y += kGravity;
}
