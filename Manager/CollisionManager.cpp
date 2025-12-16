#include "CollisionManager.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "BulletManager.h"

//プレイヤーと敵の当たり判定
bool CollisionManager::PlayerVsEnemies(const Rect& playerRect,
    const std::vector<std::shared_ptr<Enemy>>& enemies)
{
    for (const auto& enemy : enemies)
    {
        if (!enemy->IsDead() &&
            playerRect.IsCollision(enemy->GetColRect()))
        {
            return true;
        }
    }
    return false;
}

//プレイヤーと鍵の当たり判定
bool CollisionManager::PlayerVsKey(const Rect& playerRect,const Rect& keyRect)
{
    return playerRect.IsCollision(keyRect);
}

// プレイヤー弾と敵
void CollisionManager::PlayerBulletsVsEnemies(
    std::vector<std::shared_ptr<Bullet>>& bullets,
    std::vector<std::shared_ptr<Enemy>>& enemies)
{
    for (auto& bullet : bullets)
    {
        if (!bullet->IsAlive()) continue;
        if (!bullet->IsPlayerBullet()) continue;

        for (auto& enemy : enemies)
        {
            if (enemy->IsDead()) continue;

            if (bullet->GetColRect().IsCollision(enemy->GetColRect()))
            {
                enemy->OnHit(bullet->GetDamage());
                bullet->OnHit();
                break;
            }
        }
    }
}

//敵弾とプレイヤー
bool CollisionManager::EnemyBulletsVsPlayer(
    std::vector<std::shared_ptr<Bullet>>& bullets,
    Player& player)
{
    for (auto& bullet : bullets)
    {
        if (!bullet->IsAlive()) continue;
        if (bullet->IsPlayerBullet()) continue;

        if (bullet->GetColRect().IsCollision(player.GetColRect()))
        {
            bullet->OnHit();
            player.OnDamage();
            return true;
        }
    }
    return false;
}
