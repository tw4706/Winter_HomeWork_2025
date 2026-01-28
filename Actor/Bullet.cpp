#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "SeManager.h"
#include "Animation.h"
#include "Application.h"
#include "GlobalConstants.h"
#include "BulletManager.h"
#include "EffectManager.h"
#include"SpriteEffect.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//====================
	// 当たり判定・描画関連
	//====================
	constexpr float kGround = 1764.0f;
	constexpr float kScale = 1.5f;
	constexpr float kHalfColSize = 32;
	constexpr float KEnemyBulletScale = 2.0f;
	constexpr float KBossBulletScale = 3.0f;
	constexpr float kColOffsetX = 50.0f;
	constexpr float kColOffsetY = 20.0f;
	constexpr int kScreenOutOffset = 100;	//画面外判定用

	//====================
	// ステータス関連
	//====================
	constexpr int kMaxDamage = 3;
	constexpr int kMinDamage = 1;
	constexpr int kMaxHitCount = 2;
	constexpr float kGravity = 0.3f;

	//====================
	//波動関連
	//====================
	constexpr int kHadouSrcX = 192;
	constexpr int kHadouSrcY = 192;
	constexpr int kWaveNum = 3;
	constexpr float kWaveSpacing = 40.0f;
	constexpr float kWaveW = 32.0f;
	constexpr float kWaveH = 32.0f;
	constexpr float kWaveSpawnInterval = 10;
	constexpr int kWaveLifetime = 30;

	//========================
	// アニメーション関連
	//========================
	constexpr int kBulletFrameCount = 5;
	constexpr int kBulletInterval = 5;
	constexpr int kWaveFrameCount = 4;
	constexpr int kWaveInterval = 5;
	constexpr float kWaveScale = 1.0f;

	//========================
	// Torch関連
	//========================
	constexpr float kTorchGravity = 0.3f;
	constexpr float kTorchWaveOffsetY = 15.0f;
	constexpr int kTorchLandSrcW = 128;
	constexpr int kTorchLandSrcH = 32;
	constexpr int kTorchLandFrameW = 16;
	constexpr int kTorchLandFrameH = 16;
	constexpr int kTorchLandFrameCount = 4;
	constexpr int kTorchLandFrameInterval = 4;
	constexpr float kTorchLandScale = 1.0f;

	//========================
	// 衝突判定関連
	//========================
	constexpr float kLandingCheckMargin = 0.5f;

	//========================
	// 弾の回転角度関連
	//========================
	constexpr float kKnifeAngleRight = DX_PI_F / 2.0f;
	constexpr float kKnifeAngleLeft = DX_PI_F * 1.5f;

	//========================
	// エフェクト関連
	//========================
	constexpr int kBulletEffectSrcX = 240;
	constexpr int kBulletEffectSrcY = 48;
	constexpr int kEffectFrameW = 16;
	constexpr int kEffectFrameH = 16;
	constexpr int kEffectFrameCount = 4;
	constexpr int kEffectFrameInterval = 5;
	constexpr float kBulletEffectScale = 1.5f;
}

Bullet::Bullet(Vector2 pos, Vector2 vel, BulletType bulletType, std::shared_ptr<Bg>bg) :
	GameObject(pos, vel),
	isAlive_(true),
	bulletH_(-1),
	damage_(1),
	hitCount_(0),
	hadouH_(-1),
	waveDir_(1.0f),
	prevWaveDir_(1.0f),
	isWaveSpawned_(false),
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

	float scale = 1.0f;

	if (bulletType_ == BulletType::Boss1Bullet)
	{
		scale = KBossBulletScale;
	}

	animations_ = std::make_unique<Animation>(
		bulletH_,
		config.width,
		config.height,
		kBulletFrameCount, kBulletInterval,
		scale,
		true, 0);

	hadouH_ = LoadGraph("data/Bullet/hadou.png");
	assert(hadouH_ >= 0);

	colSize_ = kHalfColSize;

	//当たり判定の初期化
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);
}

//弾の種別ごとの更新処理
void Bullet::UpdateShot()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	if (!isAlive_) return;

	if (bulletType_ == BulletType::EnemyBullet|| bulletType_ == BulletType::Boss1Bullet)
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
		//貫通するだけで何もしない
	case BulletType::Lance:
		break;
	case BulletType::Torch:
		vel_.y += kGravity;

		//松明の下端がマップチップの上面に触れたら波動発生
		if (isGround_ && !isWaveSpawned_)
		{
			SpawnWave();
			vel_ = Vector2(0.0f, 0.0f);
		}
		break;
	case BulletType::EnemyBullet:
		break;
	case BulletType::Boss1Bullet:
		break;
	default:
		break;
	}
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	if (!isAlive_ && isWaveSpawned_)
	{
		UpdateWave(enemies);
		return;
	}

	//衝突判定
	CheckBulletAndMapCollision();

	// 敵弾は移動だけ
	if (bulletType_ == BulletType::EnemyBullet||bulletType_ == BulletType::Boss1Bullet)
	{
		float offsetX = (vel_.x >= 0) ? kColOffsetX : -kColOffsetX;

		pos_ += vel_;
		if (bulletType_ == BulletType::Boss1Bullet)
		{
			//ボスの弾だけ当たり判定を少し前にする
			float offsetX = (vel_.x >= 0) ? kColOffsetX : -kColOffsetX;
			colRect_.SetCenter(pos_.x + offsetX, pos_.y, colSize_, colSize_);
		}
		else
		{
			//通常の敵の弾はそのまま描画
			colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);
		}
		UpdateShot();
		return;
	}

	UpdateShot();

	if (isWaveSpawned_)
	{
		UpdateWave(enemies);
	}
}

void Bullet::Draw()
{
	//弾の種類が松明で波動が発生している場合の波動の描画
	if (bulletType_ == BulletType::Torch && isWaveSpawned_)
	{
		for (auto& h : waveRects_)
		{
			if (h.appearTimer_ > 0) continue;

			float drawX = h.rect_.GetCenterX() + cameraOffset_.x;
			float drawY = h.rect_.GetCenterY() + cameraOffset_.y;

			int frame = h.animations_->GetCurrentFrame();

			int srcX = static_cast<int>(kHadouSrcX + frame * kWaveW);
			int srcY = kHadouSrcY;

			bool flip = (waveDir_ < 0);

			DrawRectRotaGraph3(
				(int)drawX, (int)drawY,
				srcX, srcY,
				static_cast<int>(kWaveW), static_cast<int>(kWaveH),
				static_cast<int>(kWaveW / 2), static_cast<int>(kWaveH / 2),
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
			angle = (vel_.x >= 0) ? kKnifeAngleRight : kKnifeAngleLeft;
		}
		if (bulletType_ == BulletType::EnemyBullet ||
			bulletType_ == BulletType::Boss1Bullet)
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

	isAlive_ = false;//弾を消す
}

void Bullet::SpawnWave()
{
	if (isWaveSpawned_) return;

	isWaveSpawned_ = true;
	waveRects_.clear();

	waveDir_ = prevWaveDir_;

	for (int i = 0; i < kWaveNum; ++i)
	{
		Rect rect;
		rect.SetLT(pos_.x + waveDir_ * (i + 1) * kWaveSpacing,
			pos_.y - kWaveH / 2 - kTorchWaveOffsetY,
			kWaveW * kScale, kWaveH * kScale);

		//コンストラクタを使う
		Hadou h(rect, static_cast<int>(i * kWaveSpawnInterval), kWaveLifetime);

		h.animations_ = std::make_unique<Animation>(
			hadouH_,
			static_cast<int>(kWaveW),
			static_cast<int>(kWaveH),
			kWaveFrameCount,
			kWaveInterval,
			kWaveScale,
			true,
			kHadouSrcY);

		Application::GetInstance().GetSEManager().PlaySE(SE::Wave);

		waveRects_.push_back(std::move(h));
	}
}

void Bullet::UpdateWave(std::vector<std::shared_ptr<Enemy>>& enemies)
{
	if (!isWaveSpawned_) return;

	for (auto& h : waveRects_)
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
				if (hitEnemies_.count(enemy.get()) > 0)continue;
				enemy->OnHit(kMinDamage);
				hitEnemies_.insert(enemy.get());
			}
		}

		h.lifetime--;
	}

	waveRects_.erase(
		std::remove_if(waveRects_.begin(),waveRects_.end(),
			[](const Hadou& h) {return h.appearTimer_ <= 0 && h.lifetime <= 0; }),
		waveRects_.end());

	if (waveRects_.empty())
	{
		isWaveSpawned_ = false;
		isAlive_ = false;
		return;
	}
}

void Bullet::RegisterHit()
{
	hitCount_++;

	if (bulletType_ == BulletType::Lance)
		return;

	if(hitCount_ >= kMaxHitCount)
	{
		isAlive_ = false;
	}
}

bool Bullet::HasHitEnemy(Enemy* enemy) const
{
	return hitEnemies_.count(enemy) > 0;
}

void Bullet::ResetHitEnemies(Enemy* enemy)
{
	hitEnemies_.insert(enemy);
}

void Bullet::CheckBulletAndMapCollision()
{
	if (!pBg_) return;

	if (bulletType_ == BulletType::Torch && isWaveSpawned_)return;

	Vector2 prevPos = pos_;

	pos_ += vel_;
	colRect_.SetCenter(pos_.x, pos_.y, colSize_, colSize_);

	//マップに当たったら即消す
	if (pBg_->IsCollision(colRect_, chipRect_))
	{
		//松明
		if (bulletType_ == BulletType::Torch)
		{
			bool isLanding =prevPos.y + colSize_ * kLandingCheckMargin <= chipRect_.GetTop() &&vel_.y > 0;

			if (isLanding)
			{
				isGround_ = true;
				vel_ = Vector2(0.0f, 0.0f);

				//着地エフェクト
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
			else
			{
				isAlive_ = false;
				return;
			}
		}

		//マップチップに当たったら短剣や槍の着弾エフェクトを出す
		if (pEffectManager_)
		{
			pEffectManager_->AddEffect(
				std::make_shared<SpriteEffect>(
					pos_,
					"data/Effect/bullet_effect.png",
					kBulletEffectSrcX,
					kBulletEffectSrcY,
					kEffectFrameW,
					kEffectFrameH,
					kEffectFrameCount,
					kEffectFrameInterval,
					kBulletEffectScale));
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
	prevWaveDir_ = isRight ? 1.0f : -1.0f;
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