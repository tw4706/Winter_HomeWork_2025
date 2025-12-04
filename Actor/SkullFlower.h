#pragma once
#include "Enemy.h"
#include "BulletManager.h"

class SkullFlower :public Enemy
{
public:
    SkullFlower(Vector2 pos, Vector2 vel, BulletManager* bulletManager);
    ~SkullFlower();

    void Init() override {};
    void Update() override;
    void Draw() override;

    void Move()override {};

private:
    BulletManager* bulletManager_; // 弾を発射するために必要
    float shotInterval_;           // 発射間隔
    float shotTimer_;              // 経過時間カウント
    int idleH_;
};

