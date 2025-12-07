#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "GlobalConstants.h"
#include "BulletManager.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	constexpr float kGround = 1764.0f;
	constexpr float kScale = 1.5f;
}

Bullet::Bullet(Vector2 pos, Vector2 vel,BulletType bulletType, std::shared_ptr<Bg>bg) :
	GameObject(pos, vel),
	isAlive_(true),
	bulletH_(-1),
	damage_(1),
	hitCount_(0),
	isHadouSpawned_(false),
	hadouNum_(0),
	hadouSpacing_(0.0f),
	hadouWidth_(0.0f),
	hadouHeight_(0.0f),
	hadouDirection_(1.0f),
	hadouSpawnInterval_(0),
	bulletType_(bulletType),
	pBg_(bg)
{
	GameObject::SetUseGravity(false);
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	hadouNum_ = 3;
	hadouSpacing_ = 40.0f;
	hadouWidth_ = 40.0f;
	hadouHeight_=40.0f;
	hadouDirection_ = 1.0f;
	hadouSpawnInterval_=10;

	//画像の初期化(弾の種別によって画像を読み込む)
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	bulletH_ = LoadGraph(config.imagePath);
	assert(bulletH_ >= 0);

	colSize_ = config.height;

	//当たり判定の初期化
	colRect_.SetCenter(pos_.x,pos_.y, config.width, config.height);
}

//弾の種別ごとの更新処理
void Bullet::UpdateShot()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	if (!isAlive_) return;

	//弾の状態に応じて処理を分岐させる
	switch (bulletType_)
	{
		//短剣
		//短剣の場合は3回当てたら敵を倒せる
	case BulletType::Knife:
		break;
		//槍
	case BulletType::Lance:
		//貫通するだけで何もしない
		break;
	case BulletType::Torch:
		vel_.y += 0.2f;

		//松明の下端がマップチップの上面に触れたら波動発生
		if (isGround_ && !isHadouSpawned_)
		{
			SpawnHadou();
			isHadouSpawned_ = true;
			vel_ = Vector2(0.0f, 0.0f);
			return;
		}
		break;
	case BulletType::EnemyBullet:
		break;
	default:
		break;
	}
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{

	if (!isAlive_) return;

	//衝突判定
	CheckBulletAndMapCollision();

	if (bulletType_ == BulletType::EnemyBullet)
	{
		pos_ += vel_;
		colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);
		UpdateShot();
		return;
	}

	UpdateShot();

	UpdateHadou(enemies);

	for (auto& enemy : enemies)
	{
		if (enemy->IsDead()) continue;

		if (colRect_.IsCollision(enemy->GetColRect()))
		{
			bool otherBullet_ = (bulletType_ == BulletType::Lance) || (bulletType_ == BulletType::Torch);
			if (otherBullet_)
			{
				damage_ = 3;
			}

			enemy->OnHit(damage_);

			//弾側の処理
			OnHit();

			break;
		}
	}
}

void Bullet::Draw()
{
	if (bulletType_ == BulletType::Torch && isHadouSpawned_)
	{
#ifdef _DEBUG
		for (auto& hadou : hadouRects_)
		{
			// ★出現タイマーが残っているものはまだ描画しない！
			if (hadou.appearTimer > 0) continue;

			hadou.rect.DrawAndCamera(cameraOffset_, 0x00ffff, false);
		}
#endif
		return;
	}
	else if (isAlive_) 
	{
		const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

		float drawX = pos_.x + cameraOffset_.x;
		float drawY = pos_.y + cameraOffset_.y;

		//切り取り位置
		int srcX = 0;
		int srcY = 0;
		int frameW = config.width;
		int frameH = config.height;

		float angle = 0.0f;

		if (bulletType_ == BulletType::Knife || bulletType_ == BulletType::Lance)
		{
			angle = (vel_.x >= 0) ? DX_PI / 2.0f : DX_PI + DX_PI / 2.0f;
		}

		DrawRectRotaGraph(
			drawX, drawY,
			srcX, srcY,        // 切り取り開始位置
			frameW, frameH,    // 切り取りサイズ
			kScale, angle,
			bulletH_,
			TRUE);
	}
#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Bullet::OnHit()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	if (!isAlive_) return;

	isAlive_ = false;// 弾を消す
}

void Bullet::SpawnHadou()
{
	if (isHadouSpawned_) return;

	isHadouSpawned_ = true;
	hadouRects_.clear();
	hadouDirection_ = (vel_.x >= 0) ? 1.0f : -1.0f;

	for (int i = 0; i < hadouNum_; ++i)
	{
		Hadou h;
		h.rect.SetLT(pos_.x + hadouDirection_ * (i + 1) * hadouSpacing_,
			pos_.y - hadouHeight_ / 2,hadouWidth_, hadouHeight_);
		h.appearTimer = i * hadouSpawnInterval_; //タイミングをずらす
		h.lifetime = 30;                         //出現時間
		hadouRects_.push_back(h);
	}
}

void Bullet::UpdateHadou(std::vector<std::shared_ptr<Enemy>>& enemies)
{
	if (!isHadouSpawned_) return;

	for (auto& h : hadouRects_)
	{
		if (h.appearTimer > 0)
		{
			h.appearTimer--;
			continue;
		}

		//敵との判定
		for (auto& enemy : enemies)
		{
			if (enemy->IsDead()) continue;
			if (h.rect.IsCollision(enemy->GetColRect()))
				enemy->OnHit(damage_*3);
		}

		//波動の持続減らす
		h.lifetime--;
	}

	//波動を削除
	hadouRects_.erase(
		std::remove_if(hadouRects_.begin(), hadouRects_.end(),
			[](const Hadou& h) { return h.appearTimer <= 0 && h.lifetime <= 0; }),
		hadouRects_.end()
	);

	//波動がすべて消えたら弾も消す
	if (hadouRects_.empty())
	{
		isAlive_ = false;
	}
}

void Bullet::CheckBulletAndMapCollision()
{
	if (!pBg_)return;

	isGround_ = false;

	pos_.x += vel_.x;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	if (pBg_->IsCollision(colRect_, chipRect_))
	{
		float bulletLeft = pos_.x - colSize_ / 2.0f;
		float bulletRight = pos_.x + colSize_ / 2.0f;
		float tileLeft = chipRect_.GetLeft();
		float tileRight = chipRect_.GetRight();
		float bulletTop = pos_.y - colSize_ / 2.0f;
		float bulletBottom = pos_.y + colSize_ / 2.0f;
		float tileTop = chipRect_.GetTop();
		float tileBottom = chipRect_.GetBottom();

		// 横方向の衝突
		bool hitSide = (bulletRight > tileLeft && bulletLeft < tileRight &&
			bulletBottom > tileTop && bulletTop < tileBottom);

		if (hitSide)
		{
			switch (bulletType_)
			{
			case BulletType::Knife:
			case BulletType::Torch:
			case BulletType::EnemyBullet:
				isAlive_ = false;  // 横衝突で消滅
				break;
			case BulletType::Lance:
				// 横衝突は無視
				break;
			}
			return;
		}
	}

	//まず垂直方向に移動
	pos_.y += vel_.y;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	//足元だけ判定する
	Rect foot = colRect_;
	foot.top_ += 2;
	foot.bottom_ += 2;

	Rect nextRect;

	if (pBg_ && pBg_->IsCollision(foot, chipRect_))
	{
		//地面の上に乗る
		pos_.y = chipRect_.GetTop() - colSize_ / 2.0f;
		vel_.y = 0;
		isGround_ = true;
	}
}
