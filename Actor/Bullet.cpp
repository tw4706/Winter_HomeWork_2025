#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	constexpr int kGround = 500;
	constexpr float kScale = 1.5f;
}

Bullet::Bullet(Vector2 pos, Vector2 vel,BulletType bulletType) :
	GameObject(pos, vel),
	isAlive_(true),
	bulletH_(-1),
	bulletType_(bulletType)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	//画像の初期化(弾の種別によって画像を読み込む)
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	bulletH_ = LoadGraph(config.imagePath);
	assert(bulletH_ >= 0);

	//当たり判定の初期化
	colRect_.SetCenter(pos_.x,pos_.y, config.width, config.height);
}

//弾の種別ごとの更新処理
void Bullet::UpdateShot()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	//弾の状態に応じて処理を分岐させる
	switch (bulletType_)
	{
		//短剣
		//短剣の場合は2回当てたら敵を倒せる
	case BulletType::Knife:
		break;
		//槍
	case BulletType::Lance:
		//貫通するだけなので何もしない
		break;
	case BulletType::Torch:
		//地面に落下したら波動を出す
		break;
	default:
		break;
	}
	
	pos_ += vel_;

	//弾の当たり判定を更新
	colRect_.SetCenter(pos_.x,pos_.y,config.width, config.height);
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	UpdateShot();

	//敵に弾が当たった時の処理
	for (auto& enemy : enemies)
	{
		//敵に当たっていたら実行する
		if (enemy && colRect_.IsCollision(enemy->GetColRect()))
		{
			OnHit();
			if (!isAlive_)return;
		}
	}
}

void Bullet::Draw()
{
	if (!isAlive_) return;

	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	//切り取り位置
	int srcX =0;
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
	
#ifdef _DEBUG
	// ★ここで位置を表示する
	DrawFormatString(
		drawX + 20, drawY - 20,
		GetColor(255, 255, 0),
		"pos(%.1f, %.1f)", pos_.x, pos_.y
	);
#endif

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Bullet::OnHit()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	if (!config.isPiercing)
	{
		//貫通しない弾は当たったら消える
		isAlive_ = false;
	}
}
