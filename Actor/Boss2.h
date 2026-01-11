#pragma once
#include "Boss.h"
#include "Bullet.h" 

enum class WeakPointType
{
    BarrierCore,
    CenterCore,
	GroundCore
};

class Boss2 : public Boss
{
public:
    using Boss::Boss;

    void Init() override;
	void Update() override;
    void Draw()override;
    void OnHit(int damage) override;

    void SelectWeakPoint();

protected:
    void LoadResources() override {};
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
    void UpdateExposed() override;
	void UpdateHurt() override;
  
private:
    WeakPointType currentWeakPoint_;

	//ŠeŽã“_‚Ì‘Ì—Í
    int barrierHP_;
    float weakDamageRate_;
};