#pragma once
#include "Boss.h"
#include "Bullet.h" 

enum class AttackPattern
{
    Idle,
    Barrier,
	Spear,
    Hadou,
    Count
};

class Boss2 : public Boss
{
public:
    using Boss::Boss;

    void Init() override;
	void Update() override;
    void Draw()override;
    void OnHit(int damage) override;

	//各攻撃パターンの選択
    void SelectRandomAttack();
	//各攻撃パターンの関数
    void KnifeAttack();
    void SpearAttack();
    void HadouAttack();

	void SetBulletManager(BulletManager* bm) { pBulletManager_ = bm; }

protected:
    void LoadResources() override {};
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
	void UpdateHurt() override;
  
private:
    int invincibleTimer_ = 0;//無敵時間用タイマー
    int barrierInvincibleTimer_;
    int centerInvincibleTimer_;
    int groundInvincibleTimer_;
    BulletManager* pBulletManager_ = nullptr;

    AttackPattern currentAttack_ = AttackPattern::Idle;

    int barrierHP_ = 0;//バリアの耐久値
};