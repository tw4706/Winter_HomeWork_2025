#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
#include<vector>
class Input;
class Enemy;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel);
	~Bullet()override;

	void Init()override;
	void Update()override;
	void Update(Input&input,std::vector<std::shared_ptr<Enemy>>&enemies);
	void Draw()override;

	// ’e‚ªÁ‚¦‚½‚©‚Ç‚¤‚©‚ğ•Ô‚·
	bool IsAlive()const { return isAlive_; }

private:
	Vector2 pos_;//ˆÊ’u
	Vector2 vel_;//‘¬“x
	bool isAlive_;//’e‚ª‘¶İ‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
	int bulletH_;//’e‚Ì‰æ‘œƒnƒ“ƒhƒ‹
};

