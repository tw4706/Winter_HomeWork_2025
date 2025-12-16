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
		Fly,
		Hurt,
		Dead
	};
	Boss(Vector2 pos, Vector2 vel,std::shared_ptr<Player>player,
		BulletManager*bm,std::shared_ptr<Camera>camera);
	~Boss();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Move()override {};

	bool IsBoss() const override { return true; }

	void ChangeState(BossState nextState);

	void UpdateIdle();
	void UpdateAttack();
	void UpdateFly();
	void UpdateHurt();
	void UpdateDead();

	void OnHit(int damage);

private:
	int hp_;//体力
	BossState currentState_;//現在の状態
	int stateTimer_;//状態遷移用タイマー
	int handle_;//画像ハンドル
	float shotTimer_;//弾を撃つまでの時間
	float shotInterval_;//弾を撃つ間隔
	Vector2 backPos_;//戻る位置を保存する変数
	bool hasShot_;//弾を撃ったかどうかのフラグ
	int knockbackDir_;//ノックバックの方向
	bool isCharging_;     //突進中か
	Vector2 chargeVel_;   //突進速度
	bool isActive_;

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

