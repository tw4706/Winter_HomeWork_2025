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
	void Init()override {};
	void Update(Input& input, std::vector<std::shared_ptr<Enemy>>&enemies, Player&player);
	void Update()override {};
	void Draw()override;

	//画面外に出たかどうかを返す関数
	bool IsOutOfScreen(const std::shared_ptr<Bullet>& bullet) const;

	//プレイヤーの弾の種別を判定
	bool IsPlayerBullet(BulletType type)const;

	//敵の弾を追加
	void AddEnemyBullet(Vector2& pos, Vector2& vel);

	void AddBoss1Bullet(Vector2& pos, Vector2& vel);

	//プレイヤーの弾を追加
	std::vector<std::shared_ptr<Bullet>>& GetBullets();

	//ボスの弾を消す
	void ClearBossBullet();

	//カメラの設定
	void SetCamera(Camera* camera);
	void SetCameraOffset(Vector2 offset);

	//エフェクトマネージャーの設定
	void SetEffectManager(EffectManager* effectManager);
private:
	Rect screenRect_;
	std::vector<std::shared_ptr<Bullet>>bullets_;
	std::map<BulletType, int>bulletLimits_;
	Camera* pCamera_;				//カメラのポインタ
	EffectManager* pEffectManager_;	//エフェクトマネージャーのポインタ
};

