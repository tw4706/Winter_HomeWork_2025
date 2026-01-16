#include "BulletManager.h"
#include "GlobalConstants.h"
#include "Camera.h"
#include <Dxlib.h>

namespace
{
	constexpr int kBulletLimit = 3;//’e‚ÌÅ‘å”
	constexpr int kLanceLimit = 2;//‘„‚ÌÅ‘å”
	constexpr int kTorchLimit = 1;//‚½‚¢‚Ü‚Â‚ÌÅ‘å”

	//’e‚ğ‰æ–ÊŠO‚ÅÁ‚·ˆ—‚É‘«‚·—]”’
	constexpr int kMargin = 10;
}

BulletManager::BulletManager():
	GameObject(),
	pEffectManager_(nullptr)
{
	//’e‚Ì”‚ğŒˆ‚ß‚é
	bulletLimits_ = {
		{BulletType::Knife,kBulletLimit},
		{BulletType::Lance,kLanceLimit},
		{BulletType::Torch,kTorchLimit}
	};
}

BulletManager::~BulletManager()
{
}

void BulletManager::Init(std::shared_ptr<Bullet> bullets)
{
	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí—Ş
	BulletType type = bullets->GetType();

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

void BulletManager::Update(Input& input, std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	//’e‚ÌXV
	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive()) continue;

		//Bullet‘¤‚É‘S‚Ä‚Ìˆ—‚ğ”C‚¹‚é
		bullet->SetEffectManager(pEffectManager_);
		bullet->Update(input, enemies);
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
		//’e‚ª‘¶İ‚µ‚È‚¢‚È‚ç•`‰æ‚µ‚È‚¢
		if (!bullet->IsAlive()) continue;

		bullet->Draw();
	}

#ifdef _DEBUG
	screenRect_.DrawAndCamera(Vector2{ 0,0 }, 0xff0000, false);
#endif
}

bool BulletManager::IsOutOfScreen(const std::shared_ptr<Bullet>& bullet) const
{
	return false;
}

void BulletManager::SetCameraOffset(Vector2 offset)
{
	for (auto& bullet : bullets_)
	{
		bullet->SetCameraOffset(offset);
	}
}

void BulletManager::SetEffectManager(EffectManager* effectManager)
{
	pEffectManager_ = effectManager;
}

bool BulletManager::IsPlayerBullet(BulletType type) const
{
	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí•Ê‚ğ•Ô‚·
	return	type == BulletType::Knife ||
		type == BulletType::Lance||
		type == BulletType::Torch;
}

void BulletManager::AddEnemyBullet(Vector2& pos, Vector2& vel)
{
	auto bullet = std::make_shared<Bullet>(pos, vel,BulletType::EnemyBullet,pBg_);
	bullet->Init();
	bullets_.push_back(bullet);
}

std::vector<std::shared_ptr<Bullet>>& BulletManager::GetBullets()
{
	return bullets_;
}