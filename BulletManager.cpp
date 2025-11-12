#include "BulletManager.h"

namespace
{
	constexpr int kBulletMax = 3; //’e‚ÌÅ‘å”
}

void BulletManager::AddBullet(std::shared_ptr<Bullet> bullets)
{
	//’e‚ğ’Ç‰Á(Å‘å3”­‚Ü‚Å‰æ–Êã‚Éo‚¹‚é)
	if (bullets_.size() >= kBulletMax) return;

	bullets_.push_back(bullets);
}

void BulletManager::Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive())continue;
		bullet->Update();
		if (bullet->GetType() == BulletType::Player)
		{
			for (auto& enemy : enemies)
			{
				if (bullet->GetColRect().IsCollision(enemy->GetColRect()))
				{
					bullet->OnHit();
					enemy->OnHit();
					break;
				}
			}
		}
		else
		{
			if (bullet->GetColRect().IsCollision(player.GetColRect()))
			{
				bullet->OnHit();
				break;
			}
		}
	}
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
		[](const std::shared_ptr<Bullet>& bullet) { return !bullet->IsAlive(); }),
		bullets_.end());
}

void BulletManager::Draw()
{
	for (const auto& bullet : bullets_)
	{
		if (bullet->IsAlive())
		{
			bullet->Draw();
		}
	}
}
