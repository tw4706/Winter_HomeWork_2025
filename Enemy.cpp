#include "Enemy.h"
#include"Animation.h"
#include<Dxlib.h>

std::map<std::string, std::map<EnemyState, Animation::AnimationSetting>>Enemy::enemySettings_ = 
{
	//敵の種類ごとのアニメーション設定
	{"Zombie",{
		{EnemyState::Idle,{6,7}}
	}},
	{"WordDog",{
		{EnemyState::Idle,{11,11}}
	}}
};

Enemy::Enemy(Vector2 pos,Vector2 vel):
	GameObject(pos,vel),
	isTurn_(false),
	isDead_(false),
	attackH_(-1)
{

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

//アニメーションの初期化
void Enemy::InitAnimation(const std::string& enemyType, int handle, int frameW, int frameH)
{
	//敵のアニメーション設定を取得する
	auto& settings = enemySettings_[enemyType];

	//アニメーションの生成と初期化
	for (auto id : settings)
	{
		//id.first: EnemyState(状態), id.second: AnimationSetting(アニメーション設定)
		EnemyState state = id.first;
		Animation::AnimationSetting animSetting = id.second;

		//ここでアニメーションを生成
		auto anim = std::make_shared<Animation>(handle, frameW, frameH);
		anim->Init(animSetting);//アニメーションの初期化
		animMap_[state] = anim;//マップに保存
	}

	//初期状態のアニメーションを設定
	if (animMap_.count(EnemyState::Idle))
	{
		currentAnim_ = animMap_[EnemyState::Idle];
	}


}

//アニメーションの更新
void Enemy::UpdateAnimation()
{
	//アニメーションの更新
	currentAnim_->Update();
}