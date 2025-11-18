#pragma once
#include"Bullet.h"
#include"Enemy.h"
#include"Player.h"
#include<vector>
#include<memory>
#include<map>

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	void Init(std::shared_ptr<Bullet>bullets);
	void Update(std::vector<std::shared_ptr<Enemy>>&enemies, Player&player);
	void Draw();

	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí•Ê‚ğ”»’è
	bool IsPlayerBullet(PlayerBulletType type)const;

private:
	std::vector<std::shared_ptr<Bullet>>bullets_;
	std::map<PlayerBulletType, int>bulletLimits_;
};

