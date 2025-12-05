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
	//‰æ‘œ‚Ì‰Šú‰»(’e‚Ìí•Ê‚É‚æ‚Á‚Ä‰æ‘œ‚ğ“Ç‚İ‚Ş)
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	bulletH_ = LoadGraph(config.imagePath);
	assert(bulletH_ >= 0);

	//“–‚½‚è”»’è‚Ì‰Šú‰»
	colRect_.SetCenter(pos_.x+cameraOffset_.x,
		pos_.y + cameraOffset_.y, config.width, config.height);
}

//’e‚Ìí•Ê‚²‚Æ‚ÌXVˆ—
void Bullet::UpdateShot()
{
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];

	float gravity = 0.5f;

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
		//’n–Ê‚É—‰º‚µ‚½‚ç”g“®‚ğo‚·
		vel_.y += gravity;
		if (pos_.y >= kGround)
		{
			pos_.y = kGround;
			isAlive_ = false;
		}
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

	//“G‚É’e‚ª“–‚½‚Á‚½‚Ìˆ—
	for (auto& enemy : enemies)
	{
		//“G‚É“–‚½‚Á‚Ä‚¢‚½‚çÀs‚·‚é
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

	//Ø‚èæ‚èˆÊ’u
	int srcX =0;
	int srcY = 0;
	int frameW = config.width;
	int frameH = config.height;

	float angle = 0.0f;

	if (bulletType_ != BulletType::EnemyBullet)
	{
		angle = (vel_.x >= 0) ? DX_PI / 2.0f : DX_PI + (DX_PI / 2.0f);
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

	if (!config.isPiercing)
	{
		//ŠÑ’Ê‚µ‚È‚¢’e‚Í“–‚½‚Á‚½‚çÁ‚¦‚é
		isAlive_ = false;
	}
}

void Bullet::SpawnHadou()
{

}
