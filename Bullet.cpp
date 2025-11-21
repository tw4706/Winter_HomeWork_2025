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
	//‰æ‘œ‚Ì‰Šú‰»(’e‚Ìí•Ê‚É‚æ‚Á‚Ä‰æ‘œ‚ğ“Ç‚İ‚Ş)
	const auto& config = kBulletConfigs[static_cast<int>(bulletType_)];
	bulletH_ = LoadGraph(config.imagePath);
	assert(bulletH_ >= 0);
	printfDx("‰æ‘œ“Ç‚İ‚İ: %s ƒnƒ“ƒhƒ‹=%d\n", config.imagePath, bulletH_);
	//“–‚½‚è”»’è‚Ì‰Šú‰»
	colRect_.SetCenter(pos_.x, pos_.y, config.width, config.height);
}

void Bullet::Update()
{

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
	case PlayerBulletType::Knife:
		break;
		//‘„
	case PlayerBulletType::Lance:
		//ŠÑ’Ê‚·‚é‚¾‚¯‚È‚Ì‚Å‰½‚à‚µ‚È‚¢
		break;
		//‚½‚¢‚Ü‚Â
		//‚½‚¢‚Ü‚Â‚Í’n–Ê‚É—‚¿‚½‚ç”g“®‚ğo‚·
	case PlayerBulletType::Torch:
		//‰º‚É”ò‚ñ‚Å‚¢‚­
		vel_.y += 0.3f;

		//’n–Ê‚É’…’n‚µ‚½‚çÁ‚¦‚é(”g“®‚ªoŒ»)
		if (pos_.y >= kGround)
		{
			pos_.y = kGround;
			vel_ = { 0, 0 };
			// Torch–{‘Ì‚ÍÁ‚¦‚é
			isAlive_ = false;
		}

		break;
	default:
		break;
	}

	pos_ += vel_;
	//’e‚Ì“–‚½‚è”»’è‚ğXV
	colRect_.SetCenter(pos_.x, pos_.y + 20, 
		kBulletConfigs[static_cast<int>(bulletType_)].width, 
		kBulletConfigs[static_cast<int>(bulletType_)].height);

	// ‰æ–ÊŠO‚©‚ço‚½‚çÁ‚¦‚é
	if (pos_.x<0 || pos_.x>Game::kScreenWidth ||
		pos_.y<0 || pos_.y>Game::kScreenHeight)
	{
		isAlive_ = false;
	}
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
	//¶‚«‚Ä‚È‚©‚Á‚½‚ç•`‰æ‚µ‚È‚¢
	if (!isAlive_)return;
	//Šp“x‚ğŒü‚«‚É‰‚¶‚Ä•ÏX
	float angle = (vel_.x >= 0) ? DX_PI / 2.0f : DX_PI + (DX_PI / 2.0f);
	DrawRotaGraph(pos_.x, pos_.y + 20, kScale, angle, bulletH_, true);
	printfDx("’e•`‰æ: í—Ş=%d x=%f y=%f Alive=%d\n", static_cast<int>(bulletType_), pos_.x, pos_.y, isAlive_);
#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
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
