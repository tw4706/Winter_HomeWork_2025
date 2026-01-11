#include "CollisionManager.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "BulletManager.h"

//プレイヤーと敵の当たり判定
Enemy* CollisionManager::PlayerVsEnemies(const Rect& playerRect,
    const std::vector<std::shared_ptr<Enemy>>& enemies)
{
    for (const auto& enemy : enemies)
    {
        if (enemy->IsDead()) continue;

        if (playerRect.IsCollision(enemy->GetColRect()))
        {
            return enemy.get(); //当たった敵を返す
        }
    }
    return nullptr;
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
                // 敵が死んでいても hitCount は増やす
                if (bullet->GetType() == BulletType::Lance)
                {
                    bullet->RegisterHit();
                }
                else
                {
                    // ナイフ・松明は当たったら消す
                    bullet->OnHit();
                }

                // 敵が生きていればダメージ
                if (!enemy->IsDead())
                {
                    enemy->OnHit(bullet->GetDamage());
                }

                // ランスは最大ヒット数に達していなければ次の敵に当たれる
                if (bullet->GetType() != BulletType::Lance)
                {
                    // ナイフ・松明は1回でループ抜け
                    break;
                }
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
            player.OnDamage(bullet->GetPos().x);
            return true;
        }
    }
    return false;
}
