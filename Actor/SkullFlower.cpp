#include "SkullFlower.h"
#include "BulletManager.h"
#include "Bullet.h"
#include<Dxlib.h>
#include <cassert>



SkullFlower::SkullFlower(Vector2 pos, Vector2 vel, BulletManager* bulletManager)
    : Enemy(pos, vel),
    bulletManager_(bulletManager),
    shotInterval_(2.0f),
    shotTimer_(0.0f),
    flowerH_(-1)
{
}

SkullFlower::~SkullFlower()
{
}

void SkullFlower::Init()
{
    flowerH_ = LoadGraph("data/Enemy/flower_idle.png");
    assert(flowerH_ >= 0);
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
    // 当たり判定の更新
    colRect_.SetCenter(pos_.x,pos_.y-15,64, 64);
}

void SkullFlower::Draw()
{
    if (isDead_) return;
    float drawX = pos_.x + cameraOffset_.x;
    float drawY = pos_.y + cameraOffset_.y;

    // スプライト描画
    DrawRectRotaGraph3(
        static_cast<int>(drawX), static_cast<int>(drawY)-15,
        0,192,               // 描画中心
        64, 64,           // 切り取り開始
        32, 32,               // 切り取りサイズ
        2.0, 2.0, 0.0,
        flowerH_, TRUE,!isTurn_
    );

    // デバッグ用に座標表示
    DrawString(0, 360, ("X: " + std::to_string((int)drawX)).c_str(), GetColor(255, 255, 255));
    DrawString(0, 400, ("Y: " + std::to_string((int)drawY)).c_str(), GetColor(255, 255, 255));

#ifdef _DEBUG
    //当たり判定の描画
    colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}