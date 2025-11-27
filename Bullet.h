#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
#include<vector>

//ƒvƒŒƒCƒ„[‚Ì‹Ê‚Ìí•Ê
enum class PlayerBulletType
{
	Lance,
	Knife
};

//’e‚Ìî•ñ
struct BulletConfig
{
	const char* imagePath;	//‰æ‘œ
	int width;				//•
	int height;				//‚‚³
	float speed;			//’e‘¬
	float shotInterval;		//˜AËŠÔŠu
	bool isPiercing;		//ŠÑ’Ê‚·‚é‚©‚Ç‚¤‚©
};

//’e‚ÌÚ×î•ñ‚Ì”z—ñ
constexpr BulletConfig kBulletConfigs[] =
{
	{"data/Bullet/Lance.png",32,32,8.0f,30.0f,true},	//‘„
	{"data/Bullet/Knife.png",32,32,12.0f,10.0f,false}	//’ZŒ•
};

class Input;
class Enemy;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel,PlayerBulletType bulletType);
	~Bullet()override;

	void Init()override;
	void Update()override;
	void UpdateShot();
	void Update(Input&input,std::vector<std::shared_ptr<Enemy>>& enemies);
	void Draw()override;

	//“–‚½‚Á‚½‚Ìˆ—‚ğs‚¤ŠÖ”
	void OnHit();

	// ’e‚ªÁ‚¦‚½‚©‚Ç‚¤‚©‚ğ•Ô‚·
	void Destroy() { isAlive_ = false; }
	bool IsAlive() const { return isAlive_; }
	//’e‚Ìí—Ş‚ğ•Ô‚·
	const PlayerBulletType& GetType() const { return bulletType_; }

private:
	Vector2 pos_;		//ˆÊ’u
	Vector2 vel_;		//‘¬“x
	bool isAlive_;		//’e‚ª‘¶İ‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
	int bulletH_;		//’e‚Ì‰æ‘œƒnƒ“ƒhƒ‹
	PlayerBulletType bulletType_;
};

