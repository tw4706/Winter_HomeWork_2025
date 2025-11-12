#pragma once
#include"Bullet.h"
#include"Enemy.h"
#include"Player.h"
class BulletManager
{
public:
	//’e‚Ì’Ç‰Á
	void AddBullet(std::shared_ptr<Bullet>bullets);
	void Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player);
	void Draw();

private:
	std::vector<std::shared_ptr<Bullet>>bullets_;

};

