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
    void OnHitByBoss2(Bullet& bullet) override;

	void OnHitWeakPoint(WeakPointType weakType,int damage);

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

	//各弱点の当たり判定
    Rect barrierRect_;
    Rect centerRect_;
    Rect groundRect_;

	//各弱点の体力
    int barrierHP_;
    float weakDamageRate_;
};