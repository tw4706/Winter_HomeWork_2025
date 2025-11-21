#include "BulletManager.h"
#include "Camera.h"
#include <Dxlib.h>

namespace
{
	constexpr int kBulletMax = 3;//弾の最大数
	constexpr int kLanceMax = 2;//槍の最大数
	constexpr int kTorchMax = 1;//たいまつの最大数
}

BulletManager::BulletManager()
{
	//弾の数を決める
	bulletLimits_ = {
		{PlayerBulletType::Knife,3},
		{PlayerBulletType::Lance,2},
		{PlayerBulletType::Torch,1}
	};
}

BulletManager::~BulletManager()
{
}

void BulletManager::Init(std::shared_ptr<Bullet> bullets)
{
	//プレイヤーの弾の種類
	PlayerBulletType type = bullets->GetType();

	//現在の同じ弾の数をカウント
	if (bullets_.size() >= bulletLimits_[type]) return;

	bullets_.push_back(bullets);
}

void BulletManager::Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player)
{
	//弾の更新
	//弾とキャラクターの当たり判定
	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive())continue;
		//弾の更新
		bullet->Update();
		bullet->UpdateShot();
		if (bullet->GetType() == PlayerBulletType::Knife)
		{
			for (auto& enemy : enemies)
			{
				//プレイヤーの弾が敵に当たった時の処理
				if (bullet->GetColRect().IsCollision(enemy->GetColRect()))
				{
					bullet->OnHit();
					enemy->OnHit();
					break;
				}
			}
		}
		else//敵の弾がプレイヤーに当たった時の処理
		{
			if (bullet->GetColRect().IsCollision(player.GetColRect()))
			{
				bullet->OnHit();
				break;
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
		//弾が存在してるなら
		if (bullet->IsAlive())
		{
			bullet->Draw();
		}
	}
}

bool BulletManager::IsPlayerBullet(PlayerBulletType type) const
{
	//プレイヤーの弾の種別を返す
	return	type == PlayerBulletType::Knife||
			type == PlayerBulletType::Lance	||
			type == PlayerBulletType::Torch;
}
