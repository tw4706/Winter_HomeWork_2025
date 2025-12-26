#pragma once
#include "Boss.h"
#include "Bullet.h" 

class Boss2 : public Boss
{
public:
    using Boss::Boss;

    void Init() override;

protected:
    void LoadResources() override;
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;

private:
    void AttackKnife();
    void AttackLance();
    void AttackTorch();

    BulletType lastHitBulletType_ = BulletType::Knife;
};