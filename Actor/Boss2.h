#pragma once
#include "Boss.h"
#include "Bullet.h" 

class Boss2 : public Boss
{
public:
    using Boss::Boss;

    void Init() override;
	void Update() override;
    void Draw()override;
    void OnHit(int damage) override;
    void OnHit(int damage, const BulletType& type);

    void UpdateJumpAttack() override;

protected:
    void LoadResources() override {};
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override {};
    void UpdateMove() override;
	void UpdateHurt() override;
  
private:
    int hitCount_;                  //ボスが被弾した数
    int guardTimer_=0;              //ガード時間
    int jumpCoolTimer_ = 0;
    bool isJumping_=false;
    bool isBarrierActive_=true;
    int barrierGraphHandle_ = -1;
    int   shieldHitTimer_ = 0;
    float shieldAlpha_ = 0.0f;

    int  shieldHP_;
    int  shieldMaxHP_;
    bool isShieldBroken_;
    int  shieldBreakTimer_;
};