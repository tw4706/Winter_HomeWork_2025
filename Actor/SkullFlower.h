#pragma once
#include "Enemy.h"
#include "BulletManager.h"

enum class SkullFlowerState
{
    Idle,
    Hurt,
    Death
};

class SkullFlower :public Enemy
{
public:
    SkullFlower(Vector2 pos, Vector2 vel, BulletManager* bm);
    ~SkullFlower();

    void Init() override;
    void Update() override;
    void Draw() override;

    void Move()override {};

    void OnHit(int damage)override;

    void UpdateAnim();

    void Shot();

private:
    BulletManager* pBm_; // 弾を発射するために必要
    float shotInterval_;           // 発射間隔
    float shotTimer_;              // 経過時間カウント
	bool isColActive_;       //当たり判定が有効かどうか
    std::vector<int>graphHandles_;
    SkullFlowerState flowerState_;
};

