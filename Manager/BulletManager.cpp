#include "BulletManager.h"
#include "GlobalConstants.h"
#include "Camera.h"
#include <Dxlib.h>

namespace
{
	constexpr int kBulletLimit = 3;//弾の最大数
	constexpr int kLanceLimit = 2;//槍の最大数
	constexpr int kTorchLimit = 1;//たいまつの最大数
}

BulletManager::BulletManager():
	GameObject()
{
	//弾の数を決める
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
	//プレイヤーの弾の種類
	BulletType type = bullets->GetType();

	//現在の同じ弾の数をカウント
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

void BulletManager::Update(Input& input, std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	//弾の更新
	//弾とキャラクターの当たり判定
	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive()) continue;

		// Bullet 側に全ての処理を任せる
		bullet->Update(input, enemies);

		// 敵弾の場合だけプレイヤー判定
		if (!IsPlayerBullet(bullet->GetType()))
		{
			if (bullet->GetColRect().IsCollision(player.GetColRect()))
			{
				bullet->OnHit();
				player.OnDamage();
			}
		}
	}

	//弾の削除
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
		[](const std::shared_ptr<Bullet>& bullet) { return !bullet->IsAlive(); }),
		bullets_.end());
}

void BulletManager::Draw()
{
	//弾の描画
	for (const auto& bullet : bullets_)
	{
		//弾が存在しないなら描画しない
		if (!bullet->IsAlive()) continue;

		bullet->Draw();
	}
#ifdef _DEBUG
	// デバッグ用：画面矩形を赤枠で描画
	screenRect_.DrawAndCamera(Vector2{ 0,0 }, 0xff0000, false);
#endif
}

void BulletManager::SetCameraOffset(Vector2 offset)
{
	for (auto& bullet : bullets_)
	{
		bullet->SetCameraOffset(offset);
	}
}

bool BulletManager::IsPlayerBullet(BulletType type) const
{
	//プレイヤーの弾の種別を返す
	return	type == BulletType::Knife ||
		type == BulletType::Lance||
		type == BulletType::Torch;
}

void BulletManager::AddEnemyBullet(Vector2& pos, Vector2& vel)
{
	auto bullet = std::make_shared<Bullet>(pos, vel,BulletType::EnemyBullet);
	bullet->Init();
	bullets_.push_back(bullet);
}
