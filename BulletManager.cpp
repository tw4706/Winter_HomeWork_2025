#include "BulletManager.h"
#include "Camera.h"

namespace
{
	constexpr int kBulletMax = 3;//’e‚ÌÅ‘å”
	constexpr int kLanceMax = 2;//‘„‚ÌÅ‘å”
	constexpr int kTorchMax = 1;
}

BulletManager::BulletManager()
{
	//’e‚Ì”‚ğŒˆ‚ß‚é
	bulletLimits_ = {
		{PlayerBulletType::Bullet,3},
		{PlayerBulletType::Lance,2},
		{PlayerBulletType::Torch,1}
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
	if (bullets_.size() >= bulletLimits_[type]) return;

	bullets_.push_back(bullets);
}

void BulletManager::Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	//’e‚ÌXV
	//’e‚ÆƒLƒƒƒ‰ƒNƒ^[‚Ì“–‚½‚è”»’è
	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive())continue;
		bullet->Update();
		if (bullet->GetType() == PlayerBulletType::Bullet)
		{
			for (auto& enemy : enemies)
			{
				//ƒvƒŒƒCƒ„[‚Ì’e‚ª“G‚É“–‚½‚Á‚½‚Ìˆ—
				if (bullet->GetColRect().IsCollision(enemy->GetColRect()))
				{
					bullet->OnHit();
					enemy->OnHit();
					break;
				}
			}
		}
		else//“G‚Ì’e‚ªƒvƒŒƒCƒ„[‚É“–‚½‚Á‚½‚Ìˆ—
		{
			if (bullet->GetColRect().IsCollision(player.GetColRect()))
			{
				bullet->OnHit();
				break;
			}
		}
	}
	//’e‚Ìíœ
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
		[](const std::shared_ptr<Bullet>& bullet) { return !bullet->IsAlive(); }),
		bullets_.end());
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

bool BulletManager::IsPlayerBullet(PlayerBulletType type) const
{
	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí•Ê‚ğ•Ô‚·
	return	type == PlayerBulletType::Bullet||
			type == PlayerBulletType::Lance	||
			type == PlayerBulletType::Torch;
}
