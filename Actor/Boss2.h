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

	void SetBulletManager(BulletManager* bm) { pBulletManager_ = bm; }

protected:
    void LoadResources() override {};
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
	void UpdateHurt() override;
  
private:
    BulletManager* pBulletManager_ = nullptr;
};