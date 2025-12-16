#pragma once
#include"Rect.h"
#include<vector>
#include<memory>

class Player;
class Enemy;
class Bullet;
class CollisionManager
{
public:

    // ƒvƒŒƒCƒ„[ ~ “G
    static Enemy*PlayerVsEnemies(const Rect& playerRect,
        const std::vector<std::shared_ptr<Enemy>>& enemies);

    // ƒvƒŒƒCƒ„[ ~ Œ®
    static bool PlayerVsKey(const Rect& playerRect,const Rect& keyRect);

    // ƒvƒŒƒCƒ„[’e ~ “G
    static void PlayerBulletsVsEnemies(std::vector<std::shared_ptr<Bullet>>& bullets,
        std::vector<std::shared_ptr<Enemy>>& enemies);

    // “G’e ~ ƒvƒŒƒCƒ„[
    static bool EnemyBulletsVsPlayer(std::vector<std::shared_ptr<Bullet>>& bullets,
        Player& player);
private:
};

