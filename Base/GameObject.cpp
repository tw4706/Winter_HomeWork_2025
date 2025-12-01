#include"GameObject.h"

namespace
{
	constexpr float kGravity = 0.3f;  // 重力
	constexpr float kCharaSize = 32.0f;//キャラクターサイズ
}

GameObject::GameObject():
	pos_{},
	vel_{}
{
}

GameObject::GameObject(Vector2 pos,Vector2 vel):
	pos_(pos),
	vel_(vel),
	isTurn_(false),
	isGround_(false),
	drawOffset_{}
{

}

GameObject::~GameObject()
{
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
