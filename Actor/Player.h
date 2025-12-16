#pragma once
#include"GameObject.h"
#include"Enemy.h"
#include"Animation.h"
#include"Bullet.h"
#include<vector>
#include<memory>
#include<functional>

enum class PlayerState
{
	Idle,
	Attack,
	Walk,
	Jump,
	Hurt,
	Death
};

class Input;
class BulletManager;
class Player :public GameObject
{
public:
	//ゲームオーバーを通知する
	std::function<void()>onGameOver_;

	Player(Vector2 pos, Vector2 vel);
	~Player()override;

	void Init()override;
	void Update()override {};
	void Update(Input& input, BulletManager& bm);
	void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
	//弾の発射処理
	void Shot(Input& input, BulletManager& bm);

	//ダメージを受けた時の処理
	void OnDamage(float enemyX);

	void Dead();
	bool IsDead()const { return !isAlive_; }
	bool IsDeadAnimFinished()const { return isDeathAnimFinished_; }

	void UpdateState(Input& input);

private:
	std::vector<int>graphHandles_;//画像ハンドルの配列
	Vector2 initializePos_;//リスポーンしたときの初期位置保存用
	bool isJumping_;//ジャンプしているかどうか
	bool isDoubleJumping_;//ダブルジャンプ可能かどうか
	bool isDamaged_;//ダメージを受けているかどうか
	bool isTouching_;//地面に接触しているかどうか
	bool isAttacking_;//攻撃しているかどうか
	int damageTimer_;//ダメージを受けてからのタイマー
	int shotTimer_;//弾の発射間隔タイマー
	bool isAlive_;//生存しているかどうか
	bool isDeathAnimFinished_;//死亡アニメーションが終了したかどうか
	//プレイヤーの状態
	PlayerState state_;
	BulletType currentBulletType_;//現在の弾の種類
	std::vector<std::shared_ptr<Animation>>animations_;//アニメーションの配列
};

