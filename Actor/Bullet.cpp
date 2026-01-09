#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "Animation.h"
#include "GlobalConstants.h"
#include "BulletManager.h"
#include "EffectManager.h"
#include"SpriteEffect.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	constexpr float kGround = 1764.0f;
	constexpr float kScale = 1.5f;
	constexpr float KEnemyBulletScale = 2.0f;

	//弾のダメージ設定
	constexpr int kMaxDamage = 3;
	constexpr int kMinDamage = 1;

	//重力
	constexpr float kGravity = 0.2f;

	//波動関連の定数
	constexpr int kHadouSrcX = 192;
	constexpr int kHadouSrcY = 192;
	constexpr int kHadouNum = 3;
	constexpr float kHadouSpacing = 40.0f;
	constexpr float kHadouW = 32.0f;
	constexpr float kHadouH = 32.0f;
	constexpr float kHadouSpawnInterval = 10;
	constexpr int kHadouLifetime = 30;

	constexpr int kColOffsetX = 20;
	constexpr int kColOffsetY = 20;

	//画面外判定用定数
	constexpr int kScreenOutOffset = 100;
}

Bullet::Bullet(Vector2 pos, Vector2 vel, BulletType bulletType, std::shared_ptr<Bg>bg) :
	GameObject(pos, vel),
	isAlive_(true),
	bulletH_(-1),
	damage_(1),
	hitCount_(0),
	hadouH_(-1),
	hadouDir_(1.0f),
	isHadouSpawned_(false),
	bulletType_(bulletType),
	pBg_(bg),
	pEffectManager_(nullptr)
{
	GameObject::SetUseGravity(false);
}

Bullet::~Bullet()
{
	DeleteGraph(hadouH_);
}

void Bullet::Init()
{
	//画像の初期化(弾の種別によって画像を読み込む)
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	bulletH_ = LoadGraph(config.imagePath);
	assert(bulletH_ >= 0);

	if (bulletType_ == BulletType::EnemyBullet)
	{
		animations_ = std::make_unique<Animation>(
			bulletH_,
			config.width* KEnemyBulletScale,
			config.height* KEnemyBulletScale,
			5,
			5,
			1.0f,
			true,0);
	}

	hadouH_ = LoadGraph("data/Bullet/hadou.png");
	assert(hadouH_ >= 0);

	colSize_ = static_cast<float>(config.height);

	//当たり判定の初期化
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);
}

//弾の種別ごとの更新処理
void Bullet::UpdateShot()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	if (!isAlive_) return;

	if (bulletType_ == BulletType::EnemyBullet)
	{
		animations_->Update();
	}

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
		vel_.y += kGravity;

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

	// 敵弾は移動だけ
	if (bulletType_ == BulletType::EnemyBullet)
	{
		pos_ += vel_;
		colRect_.SetCenter(pos_.x-kColOffsetX, pos_.y,colSize_, colSize_);
		UpdateShot();
		return;
	}

	UpdateShot();

	UpdateHadou(enemies);
}

void Bullet::Draw()
{
	//弾の種類が松明で波動が発生している場合の波動の描画
	if (bulletType_ == BulletType::Torch && isHadouSpawned_)
	{
		for (auto& h : hadouRects_)
		{
			if (h.appearTimer_ > 0) continue;

			float drawX = h.rect_.GetCenterX() + cameraOffset_.x;
			float drawY = h.rect_.GetCenterY() + cameraOffset_.y;

			int frame = h.animations_->GetCurrentFrame();

			int srcX = static_cast<int>(kHadouSrcX + frame * kHadouW);
			int srcY = kHadouSrcY;

			bool flip = (hadouDir_ < 0);

			DrawRectRotaGraph3(
				(int)drawX, (int)drawY,
				srcX, srcY,
				static_cast<int>(kHadouW), static_cast<int>(kHadouH),
				static_cast<int>(kHadouW / 2), static_cast<int>(kHadouH / 2),
				kScale, kScale,
				0.0f,
				hadouH_,
				TRUE, flip);

#ifdef _DEBUG
			h.rect_.DrawAndCamera(cameraOffset_, 0x00ffff, false);
#endif
		}
		return;
	}
	//弾が生存している場合のみ描画
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

		//ナイフと槍の場合は画像を回転させて描画
		if (bulletType_ == BulletType::Knife || bulletType_ == BulletType::Lance)
		{
			angle = static_cast<float>((vel_.x >= 0) ? DX_PI / 2.0f : DX_PI + DX_PI / 2.0f);
		}
		if (bulletType_ == BulletType::EnemyBullet)
		{
			animations_->Draw(drawX, drawY, vel_.x < 0);
		}
		else
		{
			DrawRectRotaGraph(
				static_cast<int>(drawX), static_cast<int>(drawY),
				srcX, srcY,        //切り取り開始位置
				frameW, frameH,    //切り取りサイズ
				kScale, angle,
				bulletH_,
				TRUE);
		}
	}
#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Bullet::OnHit()
{
	if (!isAlive_) return;

	//const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	isAlive_ = false;//弾を消す
}

void Bullet::SpawnHadou()
{
	if (isHadouSpawned_) return;

	isHadouSpawned_ = true;
	hadouRects_.clear();

	hadouDir_ = prevHadouDir_;

	for (int i = 0; i < kHadouNum; ++i)
	{
		Rect rect;
		rect.SetLT(pos_.x + hadouDir_ * (i + 1) * kHadouSpacing,
			pos_.y - kHadouH / 2-20,
			kHadouW*kScale,
			kHadouH*kScale);

		//コンストラクタを使う
		Hadou h(rect, static_cast<int>(i * kHadouSpawnInterval), kHadouLifetime);

		h.animations_ = std::make_unique<Animation>(
			hadouH_,
			kHadouW,        //32
			kHadouH,        //32
			4,              //アニメーションの総フレーム
			5,              //フレーム間隔
			1.0f,
			true,
			kHadouSrcY);

		hadouRects_.push_back(std::move(h));
	}
}

void Bullet::UpdateHadou(std::vector<std::shared_ptr<Enemy>>& enemies)
{
	if (!isHadouSpawned_) return;

	for (auto& h : hadouRects_)
	{
		if (h.appearTimer_ > 0)
		{
			h.appearTimer_--;
			continue;
		}

		//アニメーション更新
		if (h.animations_)
		{
			h.animations_->Update();
		}

		for (auto& enemy : enemies)
		{
			if (enemy->IsDead()) continue;

			if (h.rect_.IsCollision(enemy->GetColRect()))
			{
				enemy->OnHit(kMinDamage);
			}
		}

		h.lifetime--;
	}

	hadouRects_.erase(
		std::remove_if(
			hadouRects_.begin(),
			hadouRects_.end(),
			[](const Hadou& h) {return h.appearTimer_ <= 0 && h.lifetime <= 0; }),
		hadouRects_.end());

	if (hadouRects_.empty())
	{
		isAlive_ = false;
	}
}

void Bullet::CheckBulletAndMapCollision()
{
	if (!pBg_) return;

	pos_ += vel_;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	//マップに当たったら即消す
	if (pBg_->IsCollision(colRect_, chipRect_))
	{
		//松明
		if (bulletType_ == BulletType::Torch)
		{
			isGround_ = true;
			vel_ = Vector2(0.0f, 0.0f);

			//松明のエフェクト
			if (pEffectManager_)
			{
				pEffectManager_->AddEffect(
					std::make_shared<SpriteEffect>(
						Vector2{ pos_.x, chipRect_.GetTop() },
						"data/Effect/torch_land.png",
						128, 32,
						16, 16,
						4,
						4,
						1.0f));
			}

			return;
		}

		//マップチップに当たったら短剣や槍の着弾エフェクトを出す
		if (pEffectManager_)
		{
			pEffectManager_->AddEffect(
				std::make_shared<SpriteEffect>(
					pos_,
					"data/Effect/bullet_effect.png",
					240, 48,
					16, 16,
					4,
					5,
					1.5f));
		}

		isAlive_ = false;
		return;
	}
}

bool Bullet::IsPlayerBullet() const
{
	return bulletType_ == BulletType::Knife ||
		bulletType_ == BulletType::Lance ||
		bulletType_ == BulletType::Torch;
}

void Bullet::SetDirection(bool isRight)
{
	prevHadouDir_ = isRight ? 1.0f : -1.0f;
}

bool Bullet::IsOutOfScreen() const
{
	if (pos_.x + cameraOffset_.x < -kScreenOutOffset || pos_.x + cameraOffset_.x > Game::kScreenWidth + kScreenOutOffset ||
		pos_.y + cameraOffset_.y < -kScreenOutOffset || pos_.y + cameraOffset_.y > Game::kScreenHeight + kScreenOutOffset)
	{
		return true;
	}
	return false;
}

int Bullet::GetDamage() const
{
	switch (bulletType_)
	{
	case BulletType::Knife:	
	case BulletType::Lance:
		return kMinDamage;
	case BulletType::Torch:
		return kMaxDamage;

	default:
		return 0;
	}
}

void Bullet::SetEffectManager(EffectManager* effect)
{
	pEffectManager_ = effect;
}
