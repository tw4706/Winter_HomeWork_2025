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
    void OnHit(int damage) override;
        
	//ボスがダメージを受けられる状態かどうかを返す関数
    bool IsDamageable() const;

	//プレイヤーが選んだ武器をボスに伝える関数
    void SetPlayerWeapon(BulletType weapon);

    //ボスの攻撃行動を確定させる関数
    void DecideAttackMode(BulletType playerWeapon);

protected:
    void LoadResources() override {};
    int GetGraphIndex(BossState state) const override;

    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
    void UpdateExposed() override;
	void UpdateHurt() override;

private:
    void AttackKnife();
    void AttackLance();
    void AttackTorch();

    BossAttackMode bossMode_;//ボスの攻撃モード
    bool isModeDecided_ = false;
    BulletType playerWeapon_= BulletType::Knife;
};