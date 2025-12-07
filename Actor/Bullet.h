#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
#include<vector>

//ƒvƒŒƒCƒ„[‚Ì‹Ê‚Ìí•Ê
enum class BulletType
{
	Lance,
	Knife,
	Torch,
	EnemyBullet
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

//”g“®‚Ìî•ñ
struct Hadou
{
	Rect rect;
	int appearTimer; // oŒ»‚Ü‚Å‚Ì‘Ò‹@ƒtƒŒ[ƒ€
	int lifetime;    // oŒ»Œã‚Ìõ–½
};

//’e‚ÌÚ×î•ñ‚Ì”z—ñ
constexpr BulletConfig kBulletConfigs[] =
{
	{"data/Bullet/Lance.png",32,32,8.0f,30.0f,true},		//‘„
	{"data/Bullet/Knife.png",32,32,12.0f,15.0f,false},		//’ZŒ•
	{"data/Bullet/Torch.png",32,32,6.0f,30.0f,false},		//‚½‚¢‚Ü‚Â
	{"data/Bullet/EnemyBullet.png",32,32,6.0f,0.0f,false}	//“G‚Ì’e
};

class Input;
class Enemy;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel,BulletType bulletType);
	~Bullet()override;

	void Init()override;
	void Update() override {};
	void UpdateShot();
	void Update(Input&input,std::vector<std::shared_ptr<Enemy>>& enemies);
	void Draw()override;

	//“–‚½‚Á‚½‚Ìˆ—‚ğs‚¤ŠÖ”
	void OnHit();

	//”g“®‚Ì¶¬
	void SpawnHadou();

	//”g“®‚ÌXVˆ—
	void UpdateHadou(std::vector<std::shared_ptr<Enemy>>& enemies);

	// ’e‚ªÁ‚¦‚½‚©‚Ç‚¤‚©‚ğ•Ô‚·
	bool IsAlive() const { return isAlive_; }
	//’e‚Ìí—Ş‚ğ•Ô‚·
	const BulletType& GetType() const { return bulletType_; }

private:
	bool isAlive_;		//’e‚ª‘¶İ‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
	int bulletH_;		//’e‚Ì‰æ‘œƒnƒ“ƒhƒ‹
	int damage_;		//’e‚Ìƒ_ƒ[ƒW—Ê
	int hitCount_;		//“–‚½‚Á‚½”
	
	//”g“®ŠÖ˜A
	bool isHadouSpawned_;			//”g“®‚ğ¶¬‚µ‚½‚©‚Ç‚¤‚©
	std::vector<Hadou> hadouRects_;	//”g“®‚Ì“–‚½‚è”»’è—p‚Ì‹éŒ`
	int hadouNum_;
	float hadouSpacing_;
	float hadouWidth_;
	float hadouHeight_;
	float hadouDirection_;
	int hadouSpawnInterval_;


	
	std::vector<int>hadouTimers_;
	BulletType bulletType_;
};

