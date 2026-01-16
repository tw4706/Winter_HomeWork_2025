#pragma once
#include"Rect.h"
#include<vector>
#include<memory>

class Player;
class Enemy;
class Bullet;
class Boss2;
class CollisionManager
{
public:

    // プレイヤー × 敵
    static Enemy*PlayerVsEnemies(const Rect& playerRect,
        const std::vector<std::shared_ptr<Enemy>>& enemies);

    //プレイヤーとBoss2
    static bool PlayerVsBoss2(const Rect& playerRect,Boss2& boss);

    static void PlayerBulletsVsBoss2(std::vector<std::shared_ptr<Bullet>>& bullets, Boss2& boss);

    // プレイヤー × 鍵
    static bool PlayerVsKey(const Rect& playerRect,const Rect& keyRect);

    // プレイヤー弾 × 敵
    static void PlayerBulletsVsEnemies(std::vector<std::shared_ptr<Bullet>>& bullets,
        std::vector<std::shared_ptr<Enemy>>& enemies);

    // 敵弾 × プレイヤー
    static bool EnemyBulletsVsPlayer(std::vector<std::shared_ptr<Bullet>>& bullets,
        Player& player);
private:
};

