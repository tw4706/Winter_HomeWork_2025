#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "GlobalConstants.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	constexpr float kScale = 1.5f;
	constexpr BulletConfig kBulletConfigs[] =
	{
		{"data/Bullet/lance.png",32,32},//槍
		{},//ナイフ
		{}//たいまつ
	};
}

Bullet::Bullet(Vector2 pos, Vector2 vel,PlayerBulletType bulletType) :
	GameObject(pos, vel),
	pos_(pos),
	vel_(vel),
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
	colRect_.SetCenter(pos_.x, pos_.y, config.width, config.height);

}

void Bullet::Update()
{
	pos_ += vel_;
	//弾の当たり判定を更新
	colRect_.SetCenter(pos_.x, pos_.y + 20, 
		kBulletConfigs[static_cast<int>(bulletType_)].width, 
		kBulletConfigs[static_cast<int>(bulletType_)].height);

	// 画面外から出たら消える
	if (pos_.x<0 || pos_.x>Game::kScreenWidth ||
		pos_.y<0 || pos_.y>Game::kScreenHeight)
	{
		isAlive_ = false;
	}
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	Update();

	//敵に弾が当たった時の処理
	for (auto& enemy : enemies)
	{
		//敵に当たっていたら実行する(1発撃ったら抜ける)
		if (enemy && colRect_.IsCollision(enemy->GetColRect()))
		{
			OnHit();
			break;
		}
	}
}

void Bullet::Draw()
{
	//生きてなかったら描画しない
	if (!isAlive_)return;
	//角度を向きに応じて変更
	float angle = (vel_.x >= 0) ? DX_PI / 2.0f : DX_PI + (DX_PI / 2.0f);
	DrawRotaGraph(pos_.x, pos_.y + 20, kScale, angle, bulletH_, true);
#ifdef _DEBUG
	colRect_.DrawScroll(pBg_->GetScrollX(), pBg_->GetScrollY(), 0xff0000, false);
#endif
}

void Bullet::OnHit()
{
	//当たったら消える
	isAlive_ = false;
	if (!isAlive_)
	{
		printfDx("当たった！");
	}
}
