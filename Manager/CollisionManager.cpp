#include "CollisionManager.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "Boss2.h"
#include "BulletManager.h"

//プレイヤーと敵の当たり判定
Enemy* CollisionManager::PlayerVsEnemies(const Rect& playerRect,const std::vector<std::shared_ptr<Enemy>>& enemies)
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

bool CollisionManager::PlayerVsBoss2(const Rect& playerRect, Boss2& boss)
{
    //ボスが死んでいたら判定しない
    if (boss.IsDead()) return false;

    if (boss.IsPunchActive() &&
        playerRect.IsCollision(boss.GetPunchRect()))
    {
        return true;
    }

    if (playerRect.IsCollision(boss.GetColRect()))
    {
        return true;
    }

    return false;
}

void CollisionManager::PlayerBulletsVsBoss2(std::vector<std::shared_ptr<Bullet>>& bullets, Boss2& boss)
{
    if (boss.IsDead()) return;

    for (auto& bullet : bullets)
    {
        if (!bullet->IsAlive()) continue;
        if (!bullet->IsPlayerBullet()) continue;

        if (bullet->GetColRect().IsCollision(boss.GetColRect()))
        {
            if (bullet->HasHitEnemy(&boss))continue;

            boss.OnHit(bullet->GetDamage(), bullet->GetType());

            if (bullet->GetType() == BulletType::Lance)
            {
                bullet->ResetHitEnemies(&boss);
            }
            else
            {
                bullet->OnHit();
            }
        }
    }
}

//プレイヤーと鍵の当たり判定
bool CollisionManager::PlayerVsKey(const Rect& playerRect,const Rect& keyRect)
{
    return playerRect.IsCollision(keyRect);
}

//プレイヤー弾と敵
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

			//ボス2には当たらないようにする
            if (dynamic_cast<Boss2*>(enemy.get()) != nullptr)
            {
                continue;
            }

            if (bullet->GetColRect().IsCollision(enemy->GetColRect()))
            {
                if (!bullet->HasHitEnemy(enemy.get()))
                {
                    // 敵にダメージを与える
                    enemy->OnHit(bullet->GetDamage());

                    if (bullet->GetType() == BulletType::Lance)
                    {
                        bullet->ResetHitEnemies(enemy.get());
                    }
                    else
                    {
                        // ナイフや松明は弾を消す
                        bullet->OnHit();
                    }

                    // ナイフ・松明は1回でループ抜け
                    if (bullet->GetType() != BulletType::Lance)
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
