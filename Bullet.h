#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
class Input;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel);
	~Bullet()override;

	void Init()override;
	void Update()override;
	void Update(Input&input);
	void Draw()override;

	// ’e‚ªÁ‚¦‚½‚©‚Ç‚¤‚©‚ğ•Ô‚·
	bool IsDead()const { return isDead_; }

private:
	Vector2 pos_;//ˆÊ’u
	Vector2 vel_;//‘¬“x
	bool isDead_;//’e‚ªÁ‚¦‚½‚©‚Ç‚¤‚©
	int bulletH_;//’e‚Ì‰æ‘œƒnƒ“ƒhƒ‹
};

