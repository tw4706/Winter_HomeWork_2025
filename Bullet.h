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
	void Update(Input&input);
	void Draw()override;

private:
	Vector2 pos_;//à íu
	Vector2 vel_;//ë¨ìx
	bool isDead_;//ãÖÇ™è¡Ç¶ÇΩÇ©Ç«Ç§Ç©
};

