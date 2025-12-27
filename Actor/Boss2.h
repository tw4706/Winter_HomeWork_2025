#pragma once
#include "Boss.h"
#include "Bullet.h" 

enum class BossAttackMode
{
    Knife,
    Lance,
    Torch
};

class Boss2 : public Boss
{
public:
    using Boss::Boss;

    void Init() override;
	void Update() override;
    void Draw()override;

protected:
    void LoadResources() override;
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;

    void SetAttackMode(BossAttackMode mode);

private:
    void AttackKnife();
    void AttackLance();
    void AttackTorch();

    BulletType lastHitBulletType_ = BulletType::Knife;
	BossAttackMode bossMode_;//ボスの攻撃モード
};