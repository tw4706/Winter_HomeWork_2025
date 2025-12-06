#include "Bullet.h"
#include "Rect.h"
#include "Enemy.h"
#include "GlobalConstants.h"
#include "BulletManager.h"
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
	hitCount_(0),
	bulletType_(bulletType)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	//‰æ‘œ‚Ì‰Šú‰»(’e‚Ìí•Ê‚É‚æ‚Á‚Ä‰æ‘œ‚ğ“Ç‚İ‚Ş)
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	bulletH_ = LoadGraph(config.imagePath);
	assert(bulletH_ >= 0);

	//“–‚½‚è”»’è‚Ì‰Šú‰»
	colRect_.SetCenter(pos_.x,pos_.y, config.width, config.height);
}

//’e‚Ìí•Ê‚²‚Æ‚ÌXVˆ—
void Bullet::UpdateShot()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	//’e‚Ìó‘Ô‚É‰‚¶‚Äˆ—‚ğ•ªŠò‚³‚¹‚é
	switch (bulletType_)
	{
		//’ZŒ•
		//’ZŒ•‚Ìê‡‚Í2‰ñ“–‚Ä‚½‚ç“G‚ğ“|‚¹‚é
	case BulletType::Knife:
		break;
		//‘„
	case BulletType::Lance:
		//ŠÑ’Ê‚·‚é‚¾‚¯‚È‚Ì‚Å‰½‚à‚µ‚È‚¢
		break;
	case BulletType::Torch:
		break;
	default:
		break;
	}
	
	pos_ += vel_;

	//’e‚Ì“–‚½‚è”»’è‚ğXV
	colRect_.SetCenter(pos_.x,pos_.y,config.width, config.height);
}

void Bullet::Update(Input& input, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	UpdateShot();

	for (auto& enemy : enemies)
	{
		if (enemy->IsDead()) continue;

		if (colRect_.IsCollision(enemy->GetColRect()))
		{
			int damage = 1;

			if (bulletType_ == BulletType::Lance) damage = 3;
			if (bulletType_ == BulletType::Torch) damage = 3;

			enemy->OnHit(damage);

			// ’e‘¤‚Ìˆ—
			OnHit();

			break;
		}
	}
}

void Bullet::Draw()
{
	if (!isAlive_) return;

	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;

	//Ø‚èæ‚èˆÊ’u
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
		srcX, srcY,        // Ø‚èæ‚èŠJnˆÊ’u
		frameW, frameH,    // Ø‚èæ‚èƒTƒCƒY
		kScale, angle,
		bulletH_,
		TRUE);

#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Bullet::OnHit()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	if (!isAlive_) return;

	isAlive_ = false;// ’e‚ğÁ‚·
}
