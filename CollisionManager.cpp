#include "CollisionManager.h"
#include "Player.h"
#include "Bg.h"
#include "Enemy.h"
#include "BulletManager.h"
#include"Rect.h"

void ColConfig::CheckPlayerAndMap(const Player& player, const Bg& bg)
{
}

void CollisionManager::CheckPlayerAndEnemy(Player& player, Enemy& enemy)
{



}

void CollisionManager::CheckBulletAndEnemy(BulletManager& bm, std::vector<std::shared_ptr<Enemy>>& enemies)
{
}

void CollisionManager::CheckPlayerEnemies(Player& player, std::vector<std::shared_ptr<Enemy>>& enemies)
{

}
