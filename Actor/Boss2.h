#pragma once
#include "Boss.h"
#include "Bullet.h" 

class Boss2 :public Boss
{
public:
	using Boss::Boss;
	virtual void Init() override;
	virtual void UpdateIdle() override;
	virtual void UpdateAttack() override;
	virtual void UpdateMove() override;
private:
	void AttackKnife();
	void AttackLance();
	void AttackTorch();

	//ç≈å„Ç…ìñÇΩÇ¡ÇΩíeÇÃéÌóﬁ
	BulletType lastHitBulletType_ = BulletType::Knife;
};

