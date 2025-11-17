#include "Enemy.h"
#include"Animation.h"
#include<Dxlib.h>


Enemy::Enemy(Vector2 pos,Vector2 vel):
	GameObject(pos,vel),
	isTurn_(false),
	isDead_(false),
	idleH_(-1),
	jumpH_(-1),
	attackH_(-1)
{
	pos_ = pos;
	vel_ = vel;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	if (isDead_)return;
	GameObject::Update();
}

void Enemy::Draw()
{
	if (isDead_)return;
}

void Enemy::SetAnimationState(EnemyState state, std::shared_ptr<Animation> anim)
{
	animMap_[state] = anim;

	if (animMap_.count(state))
	{
		//状態に応じたアニメーションを設定
		currentAnim_ = animMap_[state];
	}
}

//アニメーションの更新
void Enemy::UpdateAnimation()
{
	currentAnim_->Update();
}