#include "SkullFlower.h"
#include "BulletManager.h"
#include "Bullet.h"
#include <DxLib.h>



SkullFlower::SkullFlower(Vector2 pos, Vector2 vel, BulletManager* bulletManager)
    : Enemy(pos, vel),
    bulletManager_(bulletManager),
    shotInterval_(2.0f),
    shotTimer_(0.0f),
    idleH_(-1)
{
}

SkullFlower::~SkullFlower()
{
}

void SkullFlower::Update()
{
    if (isDead_) return;

    //弾発射タイマー更新
    shotTimer_ += 1.0f / 60.0f;

    if (shotTimer_ >= shotInterval_) {
        shotTimer_ = 0.0f;

        //弾を発射
        Vector2 bulletPos = pos_;
        Vector2 bulletVel = { -5.0f, 0.0f };

        bulletManager_->AddEnemyBullet(bulletPos, bulletVel);
    }

    Enemy::Update();// 移動＋衝突更新
}

void SkullFlower::Draw()
{
    if (isDead_) return;
    DrawCircle((int)pos_.x, (int)pos_.y, 16, GetColor(255, 0, 0), TRUE);
#ifdef _DEBUG
    //当たり判定の描画
    colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}