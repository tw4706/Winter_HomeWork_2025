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
        std::shared_ptr<Camera> camera);

    void Init() override;

protected:
    void UpdateIdle() override;
    void UpdateAttack() override;
    void UpdateMove() override;
};

