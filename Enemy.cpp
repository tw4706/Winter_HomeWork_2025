#include "Enemy.h"
#include"Animation.h"
#include<Dxlib.h>


//敵リスト
std::map<std::string, std::map<EnemyState, std::pair<int, int>>> Enemy::enemySettings_ = {
    {"Zombie", {
        {EnemyState::Idle, {6, 7}} //frameCount=6, frameInterval=7
    }},
    {"WordDog", {
        {EnemyState::Idle, {11, 11}} //frameCount=11, frameInterval=11
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



void Enemy::InitAnimation(const std::string& enemyType, int handle, int frameW, int frameH) {
    auto& settings = enemySettings_[enemyType];

    for (auto& id : settings) {
        EnemyState state = id.first;
        auto animSetting = id.second; // std::pair<int,int>

        int frameCount = animSetting.first;
        int frameInterval = animSetting.second;

        AnimationInfo info(handle, frameW, frameH, frameCount, frameInterval);
        auto anim = std::make_shared<Animation>(info);
        anim->Init();
        animMap_[state] = anim;
    }

    if (animMap_.count(EnemyState::Idle)) {
        currentAnim_ = animMap_[EnemyState::Idle];
    }
}


void Enemy::SetAnimationState(EnemyState state, std::shared_ptr<Animation> anim)
{
}



//アニメーションの更新
void Enemy::UpdateAnimation()
{
	//アニメーションの更新
	currentAnim_->Update();
}