#pragma once
#include "Boss.h"
class Player;
class BulletManager;
class Boss1 :public Boss
{
public:
    Boss1(Vector2 pos, Vector2 vel,
        std::shared_ptr<Player> player,
        BulletManager* bm,
        std::shared_ptr<Camera> camera, EffectManager* effectMgr);

	~Boss1() override;

    void Init() override;
    void Update()override;

    void LoadResources() override;

	void OnHit(int damage) override;

protected:
    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
	void UpdateHurt() override;
    int GetGraphIndex(BossState state) const override;

private:
    int escapeTimer_;//逃走用タイマー
    int knockbackDir_;//ノックバックの方向
    Vector2 chargeVel_;   //突進速度
};

