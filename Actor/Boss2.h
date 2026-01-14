#pragma once
#include "Boss.h"
#include "Bullet.h" 

enum class Boss2AttackType
{
    None,
    Barrier,
    JumpAttack
};

class Boss2 : public Boss
{
public:
    using Boss::Boss;

    void Init() override;
	void Update() override;
    void Draw()override;
    void OnHit(int damage) override;

    void UpdateGuard();
    void UpdateJumpAttack();
    //攻撃パターンをランダムで決める関数
    void DecideAttack();

protected:
    void LoadResources() override {};
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
	void UpdateHurt() override;
  
private:
    int hitCount_;//ボスが被弾した数
    int guardTimer_=0;//ガード時間
    int jumpCoolTimer_ = 0;
    bool isJumping_=false;
    int barrierHp_ = 0;
    bool isBarrierActive_=false;
    int barrierGraphHandle_ = -1;

    Boss2AttackType attackType_ = Boss2AttackType::None;
};