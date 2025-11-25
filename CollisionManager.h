#pragma once
#include<memory>
#include<vector>


struct ColConfig 
{
	bool hit = false;
	bool hitFloor = false;
	bool hitCeiling = false;
	bool hitWallLeft = false;
	bool hitWallRight = false;
	float adjustX = 0.0f;
	float adjustY = 0.0f;
};


class Bg;
class Enemy;
class Player;
class BulletManager;
class CollisionManager
{
public:

	//プレイヤーとマップの当たり判定を行う
	ColConfig CheckPlayerAndMap(const Player& player, const Bg& bg);

	//プレイヤーと敵キャラクターの当たり判定を行う
	static void CheckPlayerAndEnemy(Player& player, Enemy& enemy);

	//弾と敵キャラクターの当たり判定を行う
	static void CheckBulletAndEnemy(BulletManager& bm,
		std::vector<std::shared_ptr<Enemy>>& enemies);

	//プレイヤーと敵キャラクターの当たり判定を行う
	static void CheckPlayerEnemies(Player& player, 
		std::vector<std::shared_ptr<Enemy>>& enemies);
};

