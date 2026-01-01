#pragma once
#include"Bullet.h"
#include"Enemy.h"
#include"Player.h"
#include<vector>
#include<memory>
#include<map>

class BulletManager:public GameObject
{
public:
	BulletManager();
	~BulletManager();

	void Init(std::shared_ptr<Bullet>bullets);
	void Init()override;
	void Update(Input& input, std::vector<std::shared_ptr<Enemy>>&enemies, Player&player);
	void Update()override {};
	void Draw()override;

	void SetCameraOffset(Vector2 offset);

	//プレイヤーの弾の種別を判定
	bool IsPlayerBullet(BulletType type)const;

	//敵の弾を追加
	void AddEnemyBullet(Vector2& pos,Vector2& vel);

	//プレイヤーの弾を追加
	std::vector<std::shared_ptr<Bullet>>& GetBullets();

	void SetEffectManager(EffectManager*effectManager)
	{
		pEffectManager_ = effectManager;
	}

private:
	std::vector<std::shared_ptr<Bullet>>bullets_;
	std::map<BulletType, int>bulletLimits_;
	Rect screenRect_;
	EffectManager* pEffectManager_;//エフェクトマネージャーのポインタ
};

