#include "BulletManager.h"
#include "Camera.h"
#include <Dxlib.h>

namespace
{
	constexpr int kBulletMax = 3;//’e‚ÌÅ‘å”
	constexpr int kLanceMax = 2;//‘„‚ÌÅ‘å”
	constexpr int kTorchMax = 1;//‚½‚¢‚Ü‚Â‚ÌÅ‘å”
}

BulletManager::BulletManager():
	GameObject()
{
	//’e‚Ì”‚ğŒˆ‚ß‚é
	bulletLimits_ = {
		{PlayerBulletType::Knife,3},
		{PlayerBulletType::Lance,2}
	};
}

BulletManager::~BulletManager()
{
}

void BulletManager::Init(std::shared_ptr<Bullet> bullets)
{
	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí—Ş
	PlayerBulletType type = bullets->GetType();

	//Œ»İ‚Ì“¯‚¶’e‚Ì”‚ğƒJƒEƒ“ƒg
	int bulletCount = 0;
	for (auto& bullet : bullets_)
	{
		if (bullet->GetType() == type && bullet->IsAlive())
		{
			bulletCount++;
		}
	}

	bullets_.push_back(bullets);
}

void BulletManager::Init()
{
}

void BulletManager::Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	//’e‚ÌXV
	//’e‚ÆƒLƒƒƒ‰ƒNƒ^[‚Ì“–‚½‚è”»’è
	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive()) continue;

		bullet->UpdateShot();

		if (IsPlayerBullet(bullet->GetType()))
		{
			// ƒvƒŒƒCƒ„[’e ¨ “G‚É“–‚½‚é
			for (auto& enemy : enemies)
			{
				if (!enemy->IsDead() && bullet->GetColRect().IsCollision(enemy->GetColRect()))
				{
					bullet->OnHit();
					enemy->OnHit();
					printfDx("EnemyHit!\n");
					break;
				}
			}
		}
		else
		{
			// “G’e ¨ ƒvƒŒƒCƒ„[‚É“–‚½‚é
			if (bullet->GetColRect().IsCollision(player.GetColRect()))
			{
				bullet->OnHit();
				player.OnDamage();
				printfDx("PlayerHit!\n");
			}
		}
	}

	//’e‚Ìíœ
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
		[](const std::shared_ptr<Bullet>& bullet) { return !bullet->IsAlive(); }),
		bullets_.end());
}

void BulletManager::Update()
{
}

void BulletManager::Draw()
{
	//’e‚Ì•`‰æ
	for (const auto& bullet : bullets_)
	{
		//’e‚ª‘¶İ‚µ‚Ä‚é‚È‚ç
		if (bullet->IsAlive())
		{
			bullet->Draw();
		}
	}
}

void BulletManager::SetCameraOffset(Vector2 offset)
{
	for (auto& bullet : bullets_)
	{
		bullet->SetCameraOffset(offset);
	}
}

bool BulletManager::IsPlayerBullet(PlayerBulletType type) const
{
	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí•Ê‚ğ•Ô‚·
	return	type == PlayerBulletType::Knife ||
		type == PlayerBulletType::Lance;
}
