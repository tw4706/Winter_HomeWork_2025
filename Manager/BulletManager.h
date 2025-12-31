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

	//ƒvƒŒƒCƒ„[‚Ì’e‚Ìí•Ê‚ğ”»’è
	bool IsPlayerBullet(BulletType type)const;

	//“G‚Ì’e‚ğ’Ç‰Á
	void AddEnemyBullet(Vector2& pos,Vector2& vel);

	//ƒvƒŒƒCƒ„[‚Ì’e‚ğ’Ç‰Á
	std::vector<std::shared_ptr<Bullet>>& GetBullets();

	void SetEffectManager(std::shared_ptr<EffectManager> effectManager)
	{
		pEffectManager_ = effectManager;
	}

private:
	std::vector<std::shared_ptr<Bullet>>bullets_;
	std::map<BulletType, int>bulletLimits_;
	Rect screenRect_;
	std::shared_ptr<EffectManager> pEffectManager_;
};

