#pragma once
#include "Enemy.h"
class BulletManager;
class Boss :public Enemy
{
public:
	enum class BossState
	{
		Idle,
		Attack,
		Move,
		Exposed,
		Hurt,
		Dead
	};
	Boss(Vector2 pos, Vector2 vel,std::shared_ptr<Player>player,
		BulletManager*bm,std::shared_ptr<Camera>camera);
	virtual~Boss() = 0;

	void Init()override;
	void Update()override;
	void Draw()override;

	void Move()override {};

	bool IsBoss() const override { return true; }

	void ChangeState(BossState nextState);
	virtual void OnHit(int damage);
protected:
	//画像の読み込み
	virtual void LoadResources() = 0;
	virtual int GetGraphIndex(BossState state) const = 0;
	virtual void UpdateIdle() = 0;
	virtual void UpdateAttack() = 0;
	virtual void UpdateMove() = 0;
	virtual void UpdateExposed() = 0;
	virtual void UpdateHurt();
	virtual void UpdateDead();

protected:
	int hp_;//体力
	BossState currentState_;//現在の状態
	int stateTimer_;//状態遷移用タイマー
	float shotTimer_;//弾を撃つまでの時間
	float shotInterval_;//弾を撃つ間隔
	bool hasShot_;//弾を撃ったかどうかのフラグ
	bool isActive_;
	bool isCharging_;     //突進中か
	bool isInvincible_;//無敵状態かどうか
	Vector2 backPos_;//戻る位置を保存する変数

	//グラフィックハンドルの配列
	std::vector<int>graphHandles_;

	//プレイヤーのポインタ
	std::shared_ptr<Player>pPlayer_;

	//弾管理クラスのポインタ
	BulletManager* pBm_;

	//カメラのポインタ
	std::shared_ptr<Camera>pCamera_;

	//アニメーションの配列
	std::vector<std::shared_ptr<Animation>>animations_;
};

