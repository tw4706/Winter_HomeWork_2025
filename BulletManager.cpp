#include "BulletManager.h"

namespace
{
	constexpr int kBulletMax = 3; //弾の最大数
}

void BulletManager::AddBullet(std::shared_ptr<Bullet> bullets)
{
	//弾を追加(最大3発まで画面上に出せる)
	if (bullets_.size() >= kBulletMax) return;

	bullets_.push_back(bullets);
}

void BulletManager::Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	//弾とキャラクターの当たり判定
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
