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

    //ƒvƒŒƒCƒ„[ ~ “G
    static Enemy*PlayerVsEnemies(const Rect& playerRect,const std::vector<std::shared_ptr<Enemy>>& enemies);

    //ƒvƒŒƒCƒ„[‚ÆBoss2
    static bool PlayerVsBoss2(const Rect& playerRect,Boss2& boss);

    static void PlayerBulletsVsBoss2(std::vector<std::shared_ptr<Bullet>>& bullets, Boss2& boss);

    //ƒvƒŒƒCƒ„[’e ~ “G
    static void PlayerBulletsVsEnemies(std::vector<std::shared_ptr<Bullet>>& bullets,std::vector<std::shared_ptr<Enemy>>& enemies);

    //“G’e ~ ƒvƒŒƒCƒ„[
    static bool EnemyBulletsVsPlayer(std::vector<std::shared_ptr<Bullet>>& bullets,Player& player);
};

