#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
#include<vector>

enum class BulletType
{
	Player,//ƒvƒŒƒCƒ„[‚Ì’e
	Enemy,//“G‚Ì’e
};

class Input;
class Enemy;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel, BulletType type);
	~Bullet()override;

	void Init()override;
	void Update()override;
	void Update(Input&input,std::vector<std::shared_ptr<Enemy>>& enemies);
	void Draw()override;

	//“–‚½‚Á‚½‚Ìˆ—‚ğs‚¤ŠÖ”
	void OnHit();

	// ’e‚ªÁ‚¦‚½‚©‚Ç‚¤‚©‚ğ•Ô‚·
	void Destroy() { isAlive_ = false; }
	bool IsAlive() const { return isAlive_; }
	BulletType GetType() const { return type_; }


private:
	Vector2 pos_;		//ˆÊ’u
	Vector2 vel_;		//‘¬“x
	bool isAlive_;		//’e‚ª‘¶İ‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
	int bulletH_;		//’e‚Ì‰æ‘œƒnƒ“ƒhƒ‹
	BulletType type_;	//’e‚Ìí—Ş
};

